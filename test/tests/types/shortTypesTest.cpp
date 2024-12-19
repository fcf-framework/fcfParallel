#include "../macro.hpp"
#include <fcfParallel/parallel.hpp>

FCF_PARALLEL_UNIT(
  types_short_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    short a_sourceValue,
    FCF_PARALLEL_GLOBAL const short* a_sourceArr,
    FCF_PARALLEL_GLOBAL short* a_destArr1,
    FCF_PARALLEL_GLOBAL short* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)

FCF_PARALLEL_UNIT(
  types_unsigned_short_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    unsigned short a_sourceValue,
    FCF_PARALLEL_GLOBAL const unsigned short* a_sourceArr,
    FCF_PARALLEL_GLOBAL unsigned short* a_destArr1,
    FCF_PARALLEL_GLOBAL unsigned short* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)


void shortTypesTest(){
  std::cout << "Start shortTypesTest..." << std::endl;
  {
    typedef short value_type;
    const char* units[]={ "types_short_test", "types_unsigned_short_test" };
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 1000;
      fcf::Parallel::Executor executor;
      executor.initialize();

      std::vector<value_type> source(tsize);
      std::vector<value_type> destination1(tsize);
      std::vector<value_type> destination2(tsize);

      for(value_type i = 0; i < (value_type)tsize; ++i) {
        source[i] = i + 1;
      }
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      short value = 7;
      executor(call,
               value,
               fcf::Parallel::refArg(source, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)),
               fcf::Parallel::refArg(destination1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true)),
               fcf::Parallel::refArg(destination2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true))
              );
      for(value_type i = 0; i < (value_type)tsize; ++i) {
        FCF_PARALLEL_TEST(destination1[i] == 7);
        FCF_PARALLEL_TEST(destination2[i] == i + 1);
      }
    }
  }
  {
    typedef unsigned short value_type;
    const char* units[]={ "types_short_test", "types_unsigned_short_test" };
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 1000;
      fcf::Parallel::Executor executor;
      executor.initialize();

      std::vector<value_type> source(tsize);
      std::vector<value_type> destination1(tsize);
      std::vector<value_type> destination2(tsize);

      for(value_type i = 0; i < (value_type)tsize; ++i) {
        source[i] = i + 1;
      }
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      short value = 7;
      executor(call,
               value,
               fcf::Parallel::refArg(source, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)),
               fcf::Parallel::refArg(destination1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true)),
               fcf::Parallel::refArg(destination2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true))
              );
      for(value_type i = 0; i < (value_type)tsize; ++i) {
        FCF_PARALLEL_TEST(destination1[i] == 7);
        FCF_PARALLEL_TEST(destination2[i] == i + 1);
      }
    }
  }  
}

