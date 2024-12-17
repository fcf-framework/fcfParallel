#include <iostream>
#include "../../macro.hpp"
#include <fcfParallel/parallel.hpp>


FCF_PARALLEL_UNIT(
  types_int64_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    fcf_int64 a_sourceValue,
    FCF_PARALLEL_GLOBAL const fcf_int64* a_sourceArr,
    FCF_PARALLEL_GLOBAL fcf_int64* a_destArr1,
    FCF_PARALLEL_GLOBAL fcf_int64* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)

FCF_PARALLEL_UNIT(
  types_unsigned_int64_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    fcf_uint64 a_sourceValue,
    FCF_PARALLEL_GLOBAL const fcf_uint64* a_sourceArr,
    FCF_PARALLEL_GLOBAL fcf_uint64* a_destArr1,
    FCF_PARALLEL_GLOBAL fcf_uint64* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)


void int64TypesTest(){
  std::cout << "Start int64TypesTest..." << std::endl;
  {
    typedef fcf_int64 value_type;
    const char* units[]={"types_int64_test", "types_unsigned_int64_test"};
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
      fcf::Union state;
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      call.stat = &state;
      fcf_int64 value = 7;
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
      fcf::UnionStringifyOptions uso;
      uso.friendly = true;
      state.stringify(std::cout, uso);
      std::cout << std::endl;
    }
  }
  {
    typedef fcf_uint64 value_type;
    const char* units[]={"types_int64_test", "types_unsigned_int64_test"};
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
      fcf::Union state;
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      call.stat = &state;
      fcf_int64 value = 7;
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
