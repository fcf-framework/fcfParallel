#ifndef ___FCF__PARALLEL__INCLUDE_HRR___
#define ___FCF__PARALLEL__INCLUDE_HRR___

#include "macro.hpp"

#ifndef FCF_PARALLEL_EXTERNAL_DEPENDENCIES
  #ifdef FCF_PARALLEL_IMPLEMENTATION
    #define FCF_UNION_IMPLEMENTATION
  #endif
  #ifdef FCF_PARALLEL_EXPORT
    #ifndef FCF_UNION_EXPORT
      #define FCF_UNION_EXPORT
    #endif
  #endif
  #include "libraries/fcfUnion/union.hpp"
#else
  #include <fcfUnion/union.hpp>
#endif

#endif // #ifndef ___FCF__PARALLEL__INCLUDE_HRR___
