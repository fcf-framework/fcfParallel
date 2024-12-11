#ifndef ___FCF__PARALLEL__CALL_HPP___
#define ___FCF__PARALLEL__CALL_HPP___

#include "macro.hpp"

namespace fcf {
  namespace Parallel {

    struct Stat {
      unsigned long long packageDuration;
      unsigned long long minPackageDuration;
      unsigned long long maxPackageDuration;
    };

    struct Call {
      const char*         name;
      unsigned long long  size;
      unsigned long long  packageSize;
      bool                split;
      Stat*               stat;
      Call()
        : name(0) 
        , size(0)
        , packageSize(0) 
        , split(false)
        , stat(0){
        }
    };

  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__CALL_HPP___
