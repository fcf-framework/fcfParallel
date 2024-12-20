#include <iostream>
#include "../../parallel.hpp"
#include "macro.hpp"

FCF_PARALLEL_UNIT(
    test_unit_001,
    void FCF_PARALLEL_MAIN(const FCFParallelTask* a_task, int a_index, FCF_PARALLEL_GLOBAL int* a_array1, FCF_PARALLEL_GLOBAL int* a_array2){
      a_array1[a_task->lowIndex] = a_task->lowIndex;
      a_array2[a_task->lowIndex] = a_index;
    }
);

int main(int a_argc, char* a_argv[]){
  std::cout << "Start..." << std::endl;
  {
    fcf::Parallel::Executor executor;
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("name") == "cpu");
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("enable") == true);
    executor.getEngine("opencl").property("enable", false);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_001";
    call.size = 100000;
    call.packageSize = 1000;
    std::vector<int> arr1(call.size, 999999999);
    std::vector<int> arr2(call.size, 999999999);
    executor(call, 7, fcf::Parallel::refArg(arr1), fcf::Parallel::refArg(arr2));
    for(int i = 0; i < (int)call.size; ++i){
      FCF_PARALLEL_TEST(arr1[i] == i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }
  }
  std::cout << "Complete!" << std::endl;
  return 0;
}
