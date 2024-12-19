#include "../macro.hpp"
#include <fcfParallel/parallel.hpp>

FCF_PARALLEL_UNIT(
  types_int_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    int a_sourceValue,
    FCF_PARALLEL_GLOBAL const int* a_sourceArr,
    FCF_PARALLEL_GLOBAL fcf_int32* a_destArr1,
    FCF_PARALLEL_GLOBAL int* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)

FCF_PARALLEL_UNIT(
  types_unsigned_int_test,
  void FCF_PARALLEL_MAIN(
    const FCFParallelTask* a_task,
    unsigned int a_sourceValue,
    FCF_PARALLEL_GLOBAL const unsigned int* a_sourceArr,
    FCF_PARALLEL_GLOBAL unsigned int* a_destArr1,
    FCF_PARALLEL_GLOBAL unsigned int* a_destArr2
    ){
    a_destArr1[a_task->lowIndex] = a_sourceValue;
    a_destArr2[a_task->lowIndex] = a_sourceArr[a_task->lowIndex];
  }
)

void intTypesTest(){
  std::cout << "Start intTypesTest..." << std::endl;
  {
    const char* units[]={"types_int_test", "types_unsigned_int_test"};
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 1000;
      fcf::Parallel::Executor executor;
      executor.initialize();

      std::vector<int> source(tsize);
      std::vector<int> destination1(tsize);
      std::vector<int> destination2(tsize);

      for(int i = 0; i < (int)tsize; ++i) {
        source[i] = i + 1;
      }
      fcf::Union state;
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      call.stat = &state;
      executor(call,
               7,
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
  {
    const char* units[]={"types_int_test", "types_unsigned_int_test"};
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 1000;
      fcf::Parallel::Executor executor;
      executor.initialize();

      std::vector<unsigned int> source(tsize);
      std::vector<unsigned int> destination1(tsize);
      std::vector<unsigned int> destination2(tsize);

      for(unsigned int i = 0; i < (unsigned int)tsize; ++i) {
        source[i] = i + 1;
      }
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      executor(call,
               7,
               fcf::Parallel::refArg(source, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)),
               fcf::Parallel::refArg(destination1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true)),
               fcf::Parallel::refArg(destination2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL), fcf::Parallel::ArgUpload(true))
              );
      for(unsigned int i = 0; i < (unsigned int)tsize; ++i) {
        FCF_PARALLEL_TEST(destination1[i] == 7);
        FCF_PARALLEL_TEST(destination2[i] == i + 1);
      }
    }
  }
  {
    const char* units[]={"types_int_test", "types_unsigned_int_test"};
    for(size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
      size_t tsize = 1000;
      fcf::Parallel::Executor executor;
      executor.initialize();

      std::vector<int> source(tsize);
      std::vector<int> destination1(tsize);
      std::vector<int> destination2(tsize);
      int*             destination2Ptr = &destination2[0];

      for(int i = 0; i < (int)tsize; ++i) {
        source[i] = i + 1;
      }
      fcf::Parallel::Call call;
      call.name = units[i];
      call.size = tsize;
      executor(call,
               7,
               fcf::Parallel::valArg(&source[0], 
                                     fcf::Parallel::ArgLength(tsize)
                                    ),
               fcf::Parallel::valArg(&destination1[0],
                                     fcf::Parallel::ArgLength(tsize),
                                     fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL),
                                     fcf::Parallel::ArgUpload(true)
                                    ),
               fcf::Parallel::refArg(destination2Ptr,
                                     fcf::Parallel::ArgLength(tsize),
                                     fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL),
                                     fcf::Parallel::ArgUpload(true)
                                    )
              );
      for(int i = 0; i < (int)tsize; ++i) {
        FCF_PARALLEL_TEST(destination1[i] == 7);
        FCF_PARALLEL_TEST(destination2[i] == i + 1);
      }
    }
  }

}
