#ifndef ___FCF__PARALLEL__TASK_HPP___
#define ___FCF__PARALLEL__TASK_HPP___

#include "macro.hpp"

namespace fcf {
  namespace Parallel {

    struct Task {
      unsigned long lowIndex;
      unsigned long highIndex;
      unsigned long packageIndex;
      unsigned long long offset;
    };

  } // Parallel namespace
} // fcf namespace

typedef fcf::Parallel::Task FCFParallelTask;

#endif // #ifndef ___FCF__PARALLEL__TASK_HPP___
