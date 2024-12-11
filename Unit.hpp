#ifndef ___FCF__PARALLEL__UNIT_HPP___
#define ___FCF__PARALLEL__UNIT_HPP___

#include "macro.hpp"
#include "Arg.hpp"
#include "./Details/FunctionInfo.hpp"
#include "./Details/Function.hpp"
#include <memory>
#include <string>
#include <vector>

namespace fcf {
  namespace Parallel {

    struct Unit {
      std::string       name;
      std::string       code;
      Details::Function function;
      std::vector< std::shared_ptr<BaseArg> > args;
      inline Unit()
        : name("")
        , code ("")
        , function() {
      }
      template <typename TFunction>
      Unit(const char* a_name, const char* a_code, TFunction a_function)
        : name(a_name)
        , code (a_code)
        , function(a_function) {
          Details::FunctionInfo<TFunction>()(args);
      }
    };
    
    typedef std::shared_ptr<Unit> PUnit;

  } // Parallel namespace
} // fcf namespace


#endif // #ifndef ___FCF__PARALLEL__UNIT_HPP___
