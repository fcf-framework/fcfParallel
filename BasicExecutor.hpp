#ifndef ___FCF__PARALLEL__BASIC_EXECUTOR_HRR___
#define ___FCF__PARALLEL__BASIC_EXECUTOR_HRR___

#include <tuple>
#include <algorithm>
#include "macro.hpp"
#include "Details/TupleLoop.hpp"
#include "Details/Function.hpp"
#include "Details/Distributor.hpp"
#include "BaseEngine.hpp"

namespace fcf {
  namespace Parallel {

    template <typename... EnginePack>
    class BasicExecutor {
      public:

        BasicExecutor()
          : _init (false) {
          Details::tupleLoop(
            _engines,
            [this](BaseEngine* a_item, size_t a_index){
              _enginesv[a_index] = a_item;
            }
          );
        }

        void initialize(){
          Details::tupleLoop(
            _engines,
            [this](BaseEngine* a_item, size_t a_index){
              a_item->initialize(a_index, &_distributor);
              _enablesv[a_index] = (bool)a_item->property("enable");
            }
          );
          _init = true;
        }

        size_t getEngineCount() {
          return sizeof...(EnginePack);
        }

        BaseEngine& getEngine(const char* a_engineName){
          for(size_t i = 0; i < sizeof...(EnginePack); ++i) {
            if (_enginesv[i]->property("name").template get<std::string>() == a_engineName) {
              return *_enginesv[i];
            }
          }
          throw std::runtime_error("Engine not found");
        }

        BaseEngine& getEngine(size_t a_engineIndex){
          if (a_engineIndex < sizeof...(EnginePack)){
             return *_enginesv[a_engineIndex];
          }
          throw std::runtime_error("Engine not found");
        }

        template <typename... TArgPack>
        void operator()(const Call& a_call, TArgPack... a_argPack) {
          typedef std::tuple<typename ArgResolver<TArgPack>::arg_type ...> tuple_args_type;

          if (a_call.state){
            *a_call.state = Union(UT_MAP);
          }

          if (!a_call.name || !a_call.name[0]){
            throw std::runtime_error("The name parameter is not specified");
          }

          if (a_call.size == 0){
            throw std::runtime_error("The size parameter is not set");
          }

          if (!_init){
            throw std::runtime_error("The executor object is not initialized");
          }

          Call  reserveCall;
          const Call* call = &a_call;

          if (a_call.packageSize == 0){
            reserveCall = a_call;
            reserveCall.packageSize = a_call.size;
            call = &reserveCall;
          }

          std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;
          if (a_call.state) {
            startTimePoint = std::chrono::high_resolution_clock::now();
          }

          bool autoSplit = false;
          tuple_args_type args{ArgResolver<TArgPack>::resolve(a_argPack)...};
          BaseArg* pargs[sizeof...(TArgPack)];
          Details::tupleLoop(
            (tuple_args_type&)args,
            [&pargs, &call, &autoSplit](BaseArg* a_item, size_t a_index) {
              pargs[a_index] = a_item;
              if (a_item->upload && a_item->split == PS_NONE){
                throw std::runtime_error("No data split mode set for parameter with upload mode enabled");
              }
              if (a_item->split == PS_FULL) {
                if (a_item->length !=  a_item->splitSize * call->size) {
                  throw std::runtime_error("The amount of data does not correspond to the distribution by the task");
                }
              } else if (a_item->split == PS_PACKAGE) {
                autoSplit = true;
                if (a_item->length !=  a_item->splitSize * call->packageSize) {
                  throw std::runtime_error("The amount of data does not correspond to the distribution by the task");
                }
              }
            }
          );
          BaseArg** ppargs = (BaseArg**)&pargs[0];
          _curentArgs = &ppargs;
          _curentArgsCount = sizeof...(TArgPack);

          fcf::Parallel::Details::Distributor::Call dcall;
          dcall.name        = call->name;
          dcall.count       = call->size;
          dcall.split       = call->split == ES_AUTO ? autoSplit
                                                     : call->split == ES_ENABLE;
          dcall.packageSize = call->packageSize;
          dcall.packageTime = 0;
          dcall.function    = _handler;
          dcall.userData    = (void*)this;
          dcall.state       = call->state;

          Details::tupleLoop(
            _engines,
            [&pargs, &call, &dcall, this](BaseEngine* a_item, size_t a_index) {
              if (_enablesv[a_index]) {
                a_item->prepare(*call, dcall, (BaseArg**)&pargs[0], sizeof...(TArgPack));
              }
            }
          );

          Details::tupleLoop(
            _engines,
            [&pargs, call, this](BaseEngine* a_item, size_t a_index) {
              if (_enablesv[a_index]) {
                a_item->applyArgs(true, *call, (BaseArg**)&pargs[0], sizeof...(TArgPack));
              }
            }
          );

          _distributor.call(dcall);

          if (call->state) {
            (*call->state)["duration"] = (unsigned long long)std::chrono::duration_cast<std::chrono::nanoseconds>(
                                          std::chrono::high_resolution_clock::now() - startTimePoint
                                        ).count();
            (*call->state)["devices"] = Union(UT_VECTOR);
            Union& udevices = (*call->state)["devices"];
            Details::tupleLoop(
              _engines,
              [&udevices, &dcall](BaseEngine* a_engine, size_t a_index) {
                if (!(bool)a_engine->property("enable")) {
                  return;
                }
                if (a_engine->property("devices").is<UnionVector>()) {
                  size_t size = a_engine->property("devices").size();
                  for(size_t i = 0; i < size; ++i){
                    if (!(bool)a_engine->property("devices")[i]["enable"]){
                      return;
                    }
                    std::list<Details::Distributor::DeviceIndex>::iterator ignorIt =
                      std::find_if(dcall.ignoreDevice.begin(),
                                dcall.ignoreDevice.end(),
                                [&](const Details::Distributor::DeviceIndex& a_di){
                                  return a_di.engineIndex == a_index && a_di.deviceIndex == i;
                                }
                                );
                    if (ignorIt == dcall.ignoreDevice.end()) {
                      Union::iterator devIt = udevices.insert(Union(UT_MAP));
                      (*devIt)["engine"] = a_engine->property("name");
                      (*devIt)["engineIndex"] = a_index;
                      (*devIt)["device"] = a_engine->property("devices")[i]["name"];
                      (*devIt)["deviceIndex"] = i;
                    }
                  }
                } else {
                  std::list<Details::Distributor::DeviceIndex>::iterator ignorIt =
                    std::find_if(dcall.ignoreDevice.begin(),
                              dcall.ignoreDevice.end(),
                              [&](const Details::Distributor::DeviceIndex& a_di){
                                return a_di.engineIndex == a_index;
                              }
                              );
                  if (ignorIt == dcall.ignoreDevice.end()) {
                    Union::iterator devIt = udevices.insert(Union(UT_MAP));
                    (*devIt)["engine"] = a_engine->property("name");
                    (*devIt)["engineIndex"] = a_index;
                  }
                }

              }
            );
          }

        }

      private:
        static void _handler(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, void* a_ud) {
          BasicExecutor* pthis = (BasicExecutor*)a_ud;
          pthis->_enginesv[a_subtask.deviceIndex]->execute(a_subtask, *pthis->_curentArgs, pthis->_curentArgsCount);
        }

        std::tuple<EnginePack...>          _engines;
        BaseEngine*                        _enginesv[sizeof...(EnginePack)];
        bool                               _enablesv[sizeof...(EnginePack)];
        Details::Distributor               _distributor;
        BaseArg***                         _curentArgs;
        size_t                             _curentArgsCount;
        bool                               _init;
    };

  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__CPU_ENGINE_HPP___

