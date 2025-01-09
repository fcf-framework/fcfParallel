#ifndef ___FCF__PARALLEL__UNIT_HPP___
#define ___FCF__PARALLEL__UNIT_HPP___

#include "macro.hpp"
#include "Arg.hpp"
#include "./Details/FunctionInfo.hpp"
#include "./Details/Function.hpp"
#include "./include.hpp"
#include <memory>
#include <string>
#include <vector>

namespace fcf {
  namespace Parallel {

    struct Unit {
      std::string       name;
      Union             options;
      std::string       code;
      Details::Function function;
      std::vector< std::shared_ptr<BaseArg> > args;
      inline Unit()
        : name("")
        , code ("")
        , function() {
      }
      template <typename TFunction>
      Unit(const char* a_name, const char* a_options, const char* a_code, TFunction a_function)
        : name(a_name)
        , code (a_code)
        , function(a_function) {
          try {
            options.parse(a_options);
            if (!options.is(UT_MAP)){
              options = Union(::fcf::UT_MAP);
            }
          } catch(std::exception& e) {
            throw std::runtime_error(std::string() + "Incorrent format options for '" + a_name + "' parallel unit: " + e.what());
          }
          Details::FunctionInfo<TFunction>()(args);
      }
    };
    
    typedef std::shared_ptr<Unit> PUnit;

  } // Parallel namespace
} // fcf namespace


#endif // #ifndef ___FCF__PARALLEL__UNIT_HPP___
