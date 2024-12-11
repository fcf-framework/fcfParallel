#define FCF_PARALLEL_IMPLEMENTATION
#pragma warning(disable:4464)
#include <fcfParallel/Details/Function.hpp>
#include <iostream>

#include <mutex>
#include <functional>
#include <set>
#include <chrono>
#include <random>


#include "fillTest.ipp"
#include "FunctionTest.ipp"
#include "DistributorTest.ipp"
#include "SimpleEngineTest.ipp"
#include "splitPackageTest.ipp"
#include "FunctionDescriptorTest.ipp"

int main(int, char*[]) {
  fillTest();
  functionTest();
  distributorTest();
  functionDescriptorTest();
  simpleEngineTest();
  splitPackageTest();
  std::cout << "Complete!" << std::endl;
  return 0;
}
