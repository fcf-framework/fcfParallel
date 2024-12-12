#define FCF_PARALLEL_IMPLEMENTATION
#include <fcfParallel/Details/Function.hpp>
#include <iostream>

#include <mutex>
#include <functional>
#include <set>
#include <chrono>
#include <random>


#include "./tests/fillTest.ipp"
#include "./tests/types/intTypesTest.ipp"
#include "./tests/FunctionTest.ipp"
#include "./tests/FunctionDescriptorTest.ipp"
#include "./tests/DistributorTest.ipp"
#include "./tests/SimpleEngineTest.ipp"
#include "./tests/splitPackageTest.ipp"

int main(int, char*[]) {
  fillTest();
  functionTest();
  distributorTest();
  functionDescriptorTest();
  simpleEngineTest();
  splitPackageTest();
  intTypesTest();
  std::cout << "Complete!" << std::endl;
  return 0;
}
