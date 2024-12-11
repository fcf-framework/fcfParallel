#include "../Executor.hpp"

#include <iostream>


FCF_PARALLEL_UNIT(
    test_unit_001,
    void FCF_PARALLEL_MAIN(const FCFParallelTask* a_task, int a_index, FCF_PARALLEL_GLOBAL int* a_array1, FCF_PARALLEL_GLOBAL int* a_array2){
      a_array1[a_task->lowIndex] = a_task->lowIndex;
      a_array2[a_task->lowIndex] = a_index;
    }
);

FCF_PARALLEL_UNIT(
    test_unit_002,
    void FCF_PARALLEL_MAIN(const FCFParallelTask* a_task, int a_index, FCF_PARALLEL_GLOBAL int* a_array1,  size_t a_arrayc1){
      a_array1[a_task->lowIndex%a_arrayc1] = a_index;
    }
);


void simpleEngineTest() {
  fcf::Parallel::PUnit punit = fcf::Parallel::Registrator().get("test_unit_001");
  FCF_PARALLEL_TEST(punit->args.size() == 3);

  {
    fcf::Parallel::Executor executor;
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("name") == "cpu");
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("enable") == true);
    executor.getEngine("opencl").property("enable", false);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_001";
    call.size = 100000;
    call.split = false;
    call.packageSize = 1000;
    std::vector<int> arr1(call.size, 999999999);
    std::vector<int> arr2(call.size, 999999999);
    executor(call, 7, fcf::Parallel::refArg(arr1), fcf::Parallel::refArg(arr2));
    for(int i = 0; i < (int)call.size; ++i){
      FCF_PARALLEL_TEST(arr1[i] == i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }
  }


  {
    fcf::Parallel::Executor executor;
    executor.getEngine("cpu").property("enable", false);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_001";
    call.size = 100000;
    call.split = false;
    call.packageSize = 1000;
    std::vector<int> arr1(call.size, 999999999);
    std::vector<int> arr2(call.size, 999999999);
    executor(call,
              7,
              fcf::Parallel::refArg(arr1, fcf::Parallel::ArgUpload(true), fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)),
              fcf::Parallel::refArg(arr2, fcf::Parallel::ArgUpload(true), fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)));
    for(int i = 0; i < (int)call.size; ++i){
      FCF_PARALLEL_TEST(arr1[i] == i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }
  }

  {
    fcf::Parallel::Executor executor;
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_001";
    call.size = 100000;
    call.split = false;
    call.packageSize = 10;
    std::vector<int> arr1(call.size, 999999999);
    std::vector<int> arr2(call.size, 999999999);
    fcf::Parallel::Arg< std::vector<int> > arg2 = fcf::Parallel::refArg(arr1);
    arg2.upload = true;
    fcf::Parallel::Arg< std::vector<int> > arg3 = fcf::Parallel::refArg(arr2);
    arg3.upload = true;
    executor(call, 7,
        fcf::Parallel::refArg(arr1, fcf::Parallel::ArgUpload(true), fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL)),
        fcf::Parallel::refArg(arr2, fcf::Parallel::ArgUpload(true), fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL))
        );
    for(int i = 0; i < (int)call.size; ++i){
      FCF_PARALLEL_TEST(arr1[i] == i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }
  }

  /*
  {
    fcf::Parallel::Executor executor;
    executor.getEngine("opencl").property("enable", false);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_002";
    call.size = 100*1000*1000;
    call.split = false;
    call.packageSize = 10000;
    std::vector<int> arr1(call.packageSize, 999999999);
    executor(call, 7,
        fcf::Parallel::refArg(arr1),
        arr1.size()
        );
  }
  */
}
