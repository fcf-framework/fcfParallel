#ifndef ___FCF__PARALLEL__EXECUTOR_HRR___
#define ___FCF__PARALLEL__EXECUTOR_HRR___

#include "macro.hpp"
#include "CPUEngine.hpp"
#ifndef FCF_PARALLEL_OPENCL_DISABLE
  #include "OpenCLEngine.hpp"
#endif
#include "BasicExecutor.hpp"

namespace fcf {
  namespace Parallel {

    #ifndef FCF_PARALLEL_OPENCL_DISABLE
      typedef BasicExecutor<CPUEngine, OpenCLEngine> Executor;
    #else
      typedef BasicExecutor<CPUEngine> Executor;
    #endif

  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__EXECUTOR_HRR___

