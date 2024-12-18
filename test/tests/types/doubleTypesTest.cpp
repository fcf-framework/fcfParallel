#include "../../macro.hpp"
#include <fcfParallel/parallel.hpp>

FCF_PARALLEL_UNIT(
  types_double_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    fcf_float64 a_sourceValue,
    FCF_PARALLEL_GLOBAL const fcf_float64* a_sourceArr,
    FCF_PARALLEL_GLOBAL fcf_float64* a_destArr1,
    FCF_PARALLEL_GLOBAL fcf_float64* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)

void doubleTypesTest(){
  std::cout << "Start doubleTypesTest..." << std::endl;
  {
    typedef double value_type;
    size_t tsize = 1000;
    fcf::Parallel::Executor executor;
    executor.initialize();

    std::vector<value_type> source(tsize);
    std::vector<value_type> destination1(tsize);
    std::vector<value_type> destination2(tsize);

    for(int i = 0; i < (int)tsize; ++i) {
      source[i] = (value_type)(i + 1);
    }
    fcf::Parallel::Call call;
    call.name = "types_double_test";
    call.size = tsize;
    executor(call,
             (double)7,
             fcf::Parallel::refArg(source, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)),
             fcf::Parallel::refArg(destination1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true)),
             fcf::Parallel::refArg(destination2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true))
            );
    for(int i = 0; i < (int)tsize; ++i) {
      FCF_PARALLEL_TEST(destination1[i] == 7);
      FCF_PARALLEL_TEST(destination2[i] == i + 1);
    }
  }
}
