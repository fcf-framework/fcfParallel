#include <iostream>
#include "../macro.hpp"
#include "../../../Details/FunctionDescriptor.hpp"
void functionDescriptorTest() {
  std::cout << "Start functionDescriptorTest..." << std::endl;
  {
    std::string source = 
      "void function_test(Task info, int a_index){\n"
      "\n"
      "}\n"
      ;
    fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    FCF_PARALLEL_TEST(fd1.args == "a_index");
    FCF_PARALLEL_TEST(fd1.declArgs == "int a_index");
    fcf::Parallel::Details::FunctionDescriptor fd2(source, "function_test", false);
    FCF_PARALLEL_TEST(fd2.args == "info, a_index");
    FCF_PARALLEL_TEST(fd2.declArgs == "Task info, int a_index");
  }
  {
    std::string source = 
      "void function_test(Task info, int a_index[1]){\n"
      "\n"
      "}\n"
      ;
    fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    FCF_PARALLEL_TEST(fd1.args == "a_index[1]");
    FCF_PARALLEL_TEST(fd1.declArgs == "int a_index[1]");
  }
  {
    std::string source = 
      "void function_test(Task info, int a_index [1] [10] ){\n"
      "\n"
      "}\n"
      ;
    fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    FCF_PARALLEL_TEST(fd1.args == "a_index [1] [10]");
    FCF_PARALLEL_TEST(fd1.declArgs == "int a_index [1] [10]");
  }
  {
    std::string source = 
      "void function_test(Task info, int a_index [ 1 ][ 10] ){\n"
      "\n"
      "}\n"
      ;
    fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    FCF_PARALLEL_TEST(fd1.args == "a_index [ 1 ][ 10]");
    FCF_PARALLEL_TEST(fd1.declArgs == "int a_index [ 1 ][ 10]");
  }

  {
    std::string source = 
      "void function_test(Task info){\n"
      "\n"
      "}\n"
      ;
    fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    FCF_PARALLEL_TEST(fd1.args == "");
    FCF_PARALLEL_TEST(fd1.declArgs == "");
    fcf::Parallel::Details::FunctionDescriptor fd2(source, "function_test", false);
    FCF_PARALLEL_TEST(fd2.args == "info");
    FCF_PARALLEL_TEST(fd2.declArgs == "Task info");
  }
  {
    std::string source = 
      "void function_test(){\n"
      "\n"
      "}\n"
      ;
    std::string what;
    try {
      fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    } catch(std::exception& e){
      what = e.what();
    }
    FCF_PARALLEL_TEST(what == "Function FCF_PARALLEL_MAIN does not have arguments");
    fcf::Parallel::Details::FunctionDescriptor fd2(source, "function_test", false);
    FCF_PARALLEL_TEST(fd2.args == "");
    FCF_PARALLEL_TEST(fd2.declArgs == "");
  }
  {
    std::string source = 
      "void function_test( \t \n \r ){\n"
      "\n"
      "}\n"
      ;
    std::string what;
    try {
      fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    } catch(std::exception& e){
      what = e.what();
    }
    FCF_PARALLEL_TEST(what == "Function FCF_PARALLEL_MAIN does not have arguments");
    fcf::Parallel::Details::FunctionDescriptor fd2(source, "function_test", false);
    FCF_PARALLEL_TEST(fd2.declArgs == "");
    FCF_PARALLEL_TEST(fd2.args == "");
  }
  {
    std::string source = 
      "void function_test(Task info, int a_index{\n"
      "\n"
      "}\n"
      ;
    std::string what;
    try {
      fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", true);
    } catch(std::exception& e){
      what = e.what();
    }
    FCF_PARALLEL_TEST(what == "Failed find FCF_PARALLEL_MAIN function");
    what = "";
    try {
      fcf::Parallel::Details::FunctionDescriptor fd1(source, "function_test", false);
    } catch(std::exception& e){
      what = e.what();
    }
    FCF_PARALLEL_TEST(what == "Failed find FCF_PARALLEL_MAIN function");    
  }
}
