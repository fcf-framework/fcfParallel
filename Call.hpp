#ifndef ___FCF__PARALLEL__CALL_HPP___
#define ___FCF__PARALLEL__CALL_HPP___

#include "macro.hpp"

namespace fcf {
  namespace Parallel {

    enum EExecutionSplit {
      ES_DISABLE,
      ES_ENABLE,
      ES_AUTO
    };

    struct Call {
      const char*         name;
      unsigned long long  size;
      unsigned long long  packageSize;
      EExecutionSplit     split;
      Union*              state;
      inline Call()
        : name(0) 
        , size(0)
        , packageSize(0) 
        , split(ES_AUTO)
        , state(0){
        }
    };

  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__CALL_HPP___
