#define FCF_PARALLEL_IMPLEMENTATION
#include <fcfParallel/Details/Function.hpp>
#include <iostream>

#include <mutex>
#include <functional>
#include <set>
#include <chrono>
#include <random>


#include "./tests/fillTest.ipp"
#include "./tests/FunctionTest.ipp"
#include "./tests/FunctionDescriptorTest.ipp"
#include "./tests/SimpleEngineTest.ipp"
#include "./tests/splitPackageTest.ipp"

void distributorTest();
void charTypesTest();
void shortTypesTest();
void intTypesTest();
void int64TypesTest();
void floatTypesTest();
void doubleTypesTest();

int main(int, char*[]) {

  fillTest();
  functionTest();
  distributorTest();
  functionDescriptorTest();
  simpleEngineTest();
  splitPackageTest();
  int64TypesTest();
  intTypesTest();
  shortTypesTest();
  charTypesTest();
  floatTypesTest();
  doubleTypesTest();
  std::cout << "Complete!" << std::endl;
  return 0;
}
