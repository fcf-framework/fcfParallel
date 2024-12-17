#ifndef ___FCF__PARALLEL__CPU_ENGINE_HPP___
#define ___FCF__PARALLEL__CPU_ENGINE_HPP___

#include "macro.hpp"
#include "Details/Function.hpp"
#include "BaseEngine.hpp"
#include "Call.hpp"
#include "Task.hpp"
#include "Arg.hpp"
#include "Registrator.hpp"

namespace fcf {
  namespace Parallel {

    class CPUEngine: public BaseEngine {
      public:
        CPUEngine() {
          _properties["name"]      = "cpu";
          _properties["threads"]   = std::thread::hardware_concurrency();
        }

        virtual void initialize(size_t a_index, Details::Distributor* a_distributor) {
          if (!_properties["enable"].get<bool>()) {
            return;
          }
          int threads = _properties["threads"].get<int>();
          for(int i = 0; i < threads; ++i){
            a_distributor->addDevice(a_index, i, 1, _properties["minDuration"].get<unsigned long long>());
          }
        }

        virtual void prepare(const Call& a_call, Details::Distributor::Call& a_distributorCall, BaseArg** a_args, size_t a_argsc) {
          BaseEngine::prepare(a_call, a_distributorCall, a_args, a_argsc);
          PUnit punit = Registrator().get(a_call.name);
          _function = punit->function;
          _packageSize = a_call.packageSize;
        }

        virtual void applyArgs(bool a_first, const Call& a_call, BaseArg** a_args, size_t a_argsc){
          _args     = a_args;
          _argsc    = a_argsc;
          for(size_t i = 0; i < a_argsc; ++i) {
            _function.args[i+1] = _args[i]->pointer;
          }
        }

        virtual void execute(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, BaseArg** a_args, size_t a_argsc) {
          Task task;
          Task* ptask = &task;
          unsigned long long offset = a_subtask.offset;
          fcf::Parallel::Details::Function function = _function;
          function.args[0] = &ptask;
          for(size_t i = 0; i < a_subtask.size; ++i){
            task.lowIndex     = offset & 0xffffffff;
            task.highIndex    = offset >> 32;
            task.packageIndex = offset % _packageSize;
            ++offset;
            function();
          }
        }

      private:
        Details::Function  _function;
        BaseArg**          _args;
        size_t             _argsc;
        unsigned long long _packageSize;
    };


  } // Parallel namespace
} // fcf namespace
#endif

