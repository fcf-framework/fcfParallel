#include "../../macro.hpp"
#include <fcfParallel/parallel.hpp>

FCF_PARALLEL_UNIT(
  types_char_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    char a_sourceValue,
    FCF_PARALLEL_GLOBAL const char* a_sourceArr,
    FCF_PARALLEL_GLOBAL char* a_destArr1,
    FCF_PARALLEL_GLOBAL char* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)

FCF_PARALLEL_UNIT(
  types_unsigned_char_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    unsigned char a_sourceValue,
    FCF_PARALLEL_GLOBAL const unsigned char* a_sourceArr,
    FCF_PARALLEL_GLOBAL unsigned char* a_destArr1,
    FCF_PARALLEL_GLOBAL unsigned char* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)



void charTypesTest(){
  std::cout << "Start charTypesTest..." << std::endl;
  {
    typedef char value_type;
    const char* units[]={ "types_char_test", "types_unsigned_char_test" };
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 127;
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
      char value = 7;
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
    typedef unsigned char value_type;
    const char* units[]={ "types_char_test", "types_unsigned_char_test" };
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 127;
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
      char value = 7;
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
