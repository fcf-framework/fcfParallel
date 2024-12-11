#ifndef ___FCF__PARALLEL__BASE_ENGINE_HPP___
#define ___FCF__PARALLEL__BASE_ENGINE_HPP___

#include <string>
#include "macro.hpp"
#include "include.hpp"
#include "Details/Distributor.hpp"
#include "Call.hpp"
#include "Arg.hpp"
#include "Unit.hpp"
#include "Registrator.hpp"

namespace fcf {
  namespace Parallel {

    class BaseEngine {
      public:
        BaseEngine()
          : _properties(fcf::UT_MAP){
          _properties["enable"]      = true;
          _properties["minDuration"] = 200 * 1000;
        }
        virtual ~BaseEngine(){
        }

        Union& property(std::string a_name) {
          return _properties[a_name];
        }

        void property(std::string a_name, const Union& a_value) {
          _properties[a_name] = a_value;
        }

        virtual void initialize(size_t a_index, Details::Distributor* a_distributor) {
        }

        virtual void prepare(const Call& a_call, BaseArg** a_args, size_t a_argsc);

        virtual void applyArgs(bool a_first, const Call& a_call, BaseArg** a_args, size_t a_argsc){
        }

        virtual void execute(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, BaseArg** a_args, size_t a_argsc) {

        }
      protected:
        Union _properties;
    };

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void BaseEngine::prepare(const Call& a_call, BaseArg** a_args, size_t a_argsc) {
        PUnit punit = Registrator().get(a_call.name);
        if (punit->args.size() != a_argsc) {
          throw std::runtime_error("The number of arguments does not match the required number");
        }
        if ((a_argsc + 1) > FCF_PARALLEL_MAX_FUNCTION_ARGS) {
          throw std::runtime_error("The number of arguments exceeds the maximum value");
        }

        for(size_t i = 0; i < a_argsc; ++i) {
          if (!a_call.split) {
            if (a_args[i]->split == PS_PACKAGE) {
              throw std::runtime_error(std::string() +
                                       "Argument " +
                                       std::to_string(i+1) +
                                       " has split parameter equal to PS_PACKAGE. Call::split property must be set to true");
            }
          }
          bool   found = false;
          const char*  types1[FCF_PARALLEL_MAX_TYPE_COMPATIBLE];
          size_t typesc1;
          const char*  types2[FCF_PARALLEL_MAX_TYPE_COMPATIBLE];
          size_t typesc2;
          a_args[i]->types(types1, typesc1);
          punit->args[i]->types(types2, typesc2);
          for(size_t ti1 = 0; !found && ti1 < typesc1; ++ti1){
            for(size_t ti2 = 0; ti2 < typesc2; ++ti2){
              if (ti1 == 2) {
                a_args[i]->types(types1, typesc1);
                punit->args[i]->types(types2, typesc2);

              }
              if (strcmp(types1[ti1], types2[ti2]) == 0) {
                found = true;
                break;
              }
            }
          }
          if (!found){
            throw std::runtime_error(std::string("Type does not match for argument ") + std::to_string(i+1));
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

  } // Parallel namespace
} // fcf namespace

#endif

