#ifndef ___FCF__PARALLEL__TEST__MACRO_HPP___
#define ___FCF__PARALLEL__TEST__MACRO_HPP___

  #pragma warning(disable: 5045)

  #define FCF_PARALLEL_TEST_STRINGIZE2(x) #x
  #define FCF_PARALLEL_TEST_STRINGIZE(x) FCF_PARALLEL_TEST_STRINGIZE2(x)
  #define FCF_PARALLEL_TEST_LINE_STRING FCF_PARALLEL_TEST_STRINGIZE(__LINE__)
  #define FCF_PARALLEL_TEST(exp) \
    if (!(exp)){ \
      throw std::runtime_error(std::string() + "Test error: " + #exp + "  [FILE: " + __FILE__ + ":" + FCF_PARALLEL_TEST_LINE_STRING + "]");\
    }
  #define FCF_PARALLEL_GLOBAL
  #define FCF_PARALLEL_LOCAL

#endif
