#include <iostream>

#include <mutex>
#include <functional>
#include <set>
#include <chrono>
#include <random>

void fillTest();
void distributorTest();
void functionDescriptorTest();
void functionTest();
void charTypesTest();
void shortTypesTest();
void intTypesTest();
void int64TypesTest();
void floatTypesTest();
void doubleTypesTest();
void simpleEngineTest();
void splitPackageTest();
void exfuncTest();

int main(int, char*[]) {
  /*
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
  */
  exfuncTest();
  std::cout << "Complete!" << std::endl;
  return 0;
}
