#include <iostream>
#include "../../../Details/Function.hpp"
#include "../macro.hpp"

void test(std::string& a_dst, const char* a_source) {
  a_dst = a_source;
}

void test_ptr(std::string* a_dst, const char* a_source) {
  *a_dst = a_source;
}

void functionTest() {
  std::cout << "Start functionTest..." << std::endl;
  {
    fcf::Parallel::Details::Function func(test);
    std::string dst;
    const char* src = "test";
    func(&dst, &src);
    FCF_PARALLEL_TEST(dst == "test");
  }
  {
    fcf::Parallel::Details::Function func(test);
    std::string dst;
    const char* src = "test";
    func.args[0] = &dst;
    func.args[1] = &src;
    func();
    FCF_PARALLEL_TEST(dst == "test");
  }
  {
    fcf::Parallel::Details::Function func(test_ptr);
    std::string dst;
    std::string* pdst = &dst;
    const char* src = "test";
    func(&pdst, &src);
    FCF_PARALLEL_TEST(dst == "test");
  }
}
