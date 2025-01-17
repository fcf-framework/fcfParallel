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
#endif

#ifndef FCF_PARALLEL_EXTERNAL_DEPENDENCIES
  #ifdef FCF_PARALLEL_IMPLEMENTATION
    #define FCF_BASIS_IMPLEMENTATION
  #endif
  #ifdef FCF_PARALLEL_EXPORT
    #ifndef FCF_BASIS_EXPORT
      #define FCF_BASIS_EXPORT
    #endif
  #endif
#endif

#if defined(FCF_PARALLEL_INCLUDE_LIBRARY) && defined(FCF_PARALLEL_INCLUDE_FILE)
  #ifndef FCF_PARALLEL_EXTERNAL_DEPENDENCIES
    #define FCF_PARALLEL_INCLUDE_LINE FCF_PARALLEL_STR(libraries/FCF_PARALLEL_INCLUDE_LIBRARY/FCF_PARALLEL_INCLUDE_FILE)
  #else
    #define FCF_PARALLEL_INCLUDE_LINE <libraries/FCF_PARALLEL_INCLUDE_LIBRARY/FCF_PARALLEL_INCLUDE_FILE>
  #endif
  #include FCF_PARALLEL_INCLUDE_LINE
  #undef FCF_PARALLEL_INCLUDE_LIBRARY
  #undef FCF_PARALLEL_INCLUDE_FILE
  #undef FCF_PARALLEL_INCLUDE_LINE
#else
  #ifndef FCF_PARALLEL_EXTERNAL_DEPENDENCIES
    #include "libraries/fcfBasis/basis.hpp"
    #include "libraries/fcfUnion/union.hpp"
  #else
    #include <fcfUnion/union.hpp>
    #include <fcfBasis/basis.hpp>
  #endif
#endif

#endif // #ifndef ___FCF__PARALLEL__INCLUDE_HRR___
