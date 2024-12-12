#include "../../Executor.hpp"


FCF_PARALLEL_UNIT(
    test_unit_split_pack_001,
    void FCF_PARALLEL_MAIN(const FCFParallelTask* a_task, int a_index, FCF_PARALLEL_GLOBAL int* a_array1, FCF_PARALLEL_GLOBAL int* a_array2){
      a_array1[a_task->packageIndex] = a_task->lowIndex;
      a_array2[a_task->packageIndex] = a_index;
    }
);

void splitPackageTest() {
  {
    fcf::Parallel::Executor executor;
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("name") == "cpu");
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("enable") == true);
    executor.getEngine("opencl").property("enable", false);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_split_pack_001";
    call.size = 100000;
    call.split = true;
    call.packageSize = 1000;
    std::vector<int> arr1(call.packageSize, 999999999);
    std::vector<int> arr2(call.packageSize, 999999999);
    executor(call,
              7,
              fcf::Parallel::refArg(arr1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true)),
              fcf::Parallel::refArg(arr2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true) )
              );
    int loffset = call.size - call.packageSize;

    for(int i = 0; i < (int)call.packageSize; ++i) {
      FCF_PARALLEL_TEST(arr1[i] == loffset + i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }

  }
  {
    fcf::Parallel::Executor executor;
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("name") == "cpu");
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("enable") == true);
    executor.getEngine("opencl").property("enable", false);
    executor.getEngine("cpu").property("threads", 4);
    executor.initialize();
    fcf::Parallel::Call call;
    fcf::Parallel::Stat stat;
    call.name = "test_unit_split_pack_001";
    call.size = 1000*1000*100;
    call.split = true;
    call.stat = &stat;
    call.packageSize = 1000*1000;
    std::vector<int> arr1(call.packageSize, 999999999);
    std::vector<int> arr2(call.packageSize, 999999999);

    //std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    executor(call,
              7,
              fcf::Parallel::refArg(arr1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true)),
              fcf::Parallel::refArg(arr2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true) )
              );
    //std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    //std::cout << "Total duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"<< std::endl;
    //std::cout << "Result stat [ minPackageDuration: " << stat.minPackageDuration <<
    //                         "; maxPackageDuration: " << stat.maxPackageDuration <<
    //                         "; packageDuration: "    << stat.packageDuration <<
    //                         " ]" << std::endl;

    int loffset = call.size - call.packageSize;

    for(int i = 0; i < (int)call.packageSize; ++i) {
      FCF_PARALLEL_TEST(arr1[i] == loffset + i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }
  }
  {
    fcf::Parallel::Executor executor;
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("name") == "cpu");
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("enable") == true);
    executor.getEngine("cpu").property("enable", false);
    executor.getEngine("cpu").property("threads", 4);
    executor.initialize();
    fcf::Parallel::Call call;
    fcf::Parallel::Stat stat;
    call.name = "test_unit_split_pack_001";
    call.size = 1000*1000*100;
    call.split = true;
    call.stat = &stat;
    call.packageSize = 100*1000;
    std::vector<int> arr1(call.packageSize, 999999999);
    std::vector<int> arr2(call.packageSize, 999999999);

    //std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    executor(call,
              7,
              fcf::Parallel::refArg(arr1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true)),
              fcf::Parallel::refArg(arr2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true) )
              );
    //std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    //std::cout << "Total duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"<< std::endl;
    //std::cout << "Result stat [ minPackageDuration: " << stat.minPackageDuration <<
    //                         "; maxPackageDuration: " << stat.maxPackageDuration <<
    //                         "; packageDuration: "    << stat.packageDuration <<
    //                         " ]" << std::endl;
    int loffset = call.size - call.packageSize;
    for(int i = 0; i < (int)call.packageSize; ++i) {
      FCF_PARALLEL_TEST(arr1[i] == loffset + i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }

  }

  {
    fcf::Parallel::Executor executor;
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("name") == "cpu");
    FCF_PARALLEL_TEST(executor.getEngine("cpu").property("enable") == true);
    executor.initialize();
    fcf::Parallel::Call call;
    fcf::Parallel::Stat stat;
    call.name = "test_unit_split_pack_001";
    call.size = 1000*1000*100;
    call.split = true;
    call.stat = &stat;
    call.packageSize = 1000*1000;
    std::vector<int> arr1(call.packageSize, 999999999);
    std::vector<int> arr2(call.packageSize, 999999999);

    //std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    executor(call,
              7,
              fcf::Parallel::refArg(arr1, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true)),
              fcf::Parallel::refArg(arr2, fcf::Parallel::ArgSplit(fcf::Parallel::PS_PACKAGE), fcf::Parallel::ArgUpload(true) )
              );
    //std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    //std::cout << "Total duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms"<< std::endl;
    //std::cout << "Result stat [ minPackageDuration: " << stat.minPackageDuration <<
    //                         "; maxPackageDuration: " << stat.maxPackageDuration <<
    //                         "; packageDuration: "    << stat.packageDuration <<
    //                         " ]" << std::endl;
    int loffset = call.size - call.packageSize;
    for(int i = 0; i < (int)call.packageSize; ++i) {
      FCF_PARALLEL_TEST(arr1[i] == loffset + i);
      FCF_PARALLEL_TEST(arr2[i] == 7);
    }
  }
}
