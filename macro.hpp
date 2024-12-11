#ifndef ___FCF_PARALLEL__MACRO_HPP___
#define ___FCF_PARALLEL__MACRO_HPP___


#ifdef FCF_IMPLEMENTATION 
  #define FCF_PARALLEL_IMPLEMENTATION
#endif

#ifdef FCF_EXTERNAL_DEPENDENCIES
  #define FCF_PARALLEL_EXTERNAL_DEPENDENCIES
#endif

#define FCF_PARALLEL_UNIT(a_name, a_code) \
  namespace fcf_parallel_unit_nn_##a_name { \
    using namespace std;\
    a_code; \
  } \
  namespace { fcf::Parallel::Registrator __fcf_parallel_unit_reg_##a_name(#a_name, #a_code, fcf_parallel_unit_nn_##a_name::FCF_PARALLEL_MAIN);  }

#define FCF_PARALLEL_GLOBAL

#endif
