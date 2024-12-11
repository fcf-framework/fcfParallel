#ifndef ___FCF__PARALLEL__EXECUTOR_HRR___
#define ___FCF__PARALLEL__EXECUTOR_HRR___

#include "macro.hpp"
#include "CPUEngine.hpp"
#include "OpenCLEngine.hpp"
#include "BasicExecutor.hpp"

namespace fcf {
  namespace Parallel {

    typedef BasicExecutor<CPUEngine, OpenCLEngine> Executor;

  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__EXECUTOR_HRR___

