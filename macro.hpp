#ifndef ___FCF_PARALLEL__MACRO_HPP___
#define ___FCF_PARALLEL__MACRO_HPP___

#ifdef FCF_OPENCL_DISABLE
  #ifndef FCF_PARALLEL_OPENCL_DISABLE
    #define FCF_PARALLEL_OPENCL_DISABLE
  #endif
#endif

#ifdef FCF_IMPLEMENTATION 
  #ifndef FCF_PARALLEL_IMPLEMENTATION
    #define FCF_PARALLEL_IMPLEMENTATION
  #endif
#endif

#ifdef FCF_EXPORT
  #ifndef FCF_PARALLEL_EXPORT
    #define FCF_PARALLEL_EXPORT
  #endif
#endif

#ifdef FCF_IMPORT
  #ifndef FCF_PARALLEL_IMPORT
    #define FCF_PARALLEL_IMPORT
  #endif
#endif

#ifdef FCF_EXTERNAL_DEPENDENCIES
  #ifndef FCF_PARALLEL_EXTERNAL_DEPENDENCIES
    #define FCF_PARALLEL_EXTERNAL_DEPENDENCIES
  #endif
#endif

#ifndef FCF_PARALLEL_DELC_EXTERN
  #ifdef FCF_PARALLEL_IMPLEMENTATION
    #define FCF_PARALLEL_DELC_EXTERN
  #else
    #define FCF_PARALLEL_DELC_EXTERN extern
  #endif
#endif

#ifndef FCF_PARALLEL_DECL_EXPORT
  #ifdef WIN32
    #if defined(FCF_PARALLEL_EXPORT)
      #define FCF_PARALLEL_DECL_EXPORT __declspec(dllexport)
    #elif defined(FCF_PARALLEL_IMPORT)
      #define FCF_PARALLEL_DECL_EXPORT __declspec(dllimport)
    #else
      #define FCF_PARALLEL_DECL_EXPORT
    #endif
  #else
      #define FCF_PARALLEL_DECL_EXPORT
  #endif
#endif

#define FCF_PARALLEL_UNIT(a_name, a_options, a_code) \
  namespace fcf_parallel_unit_nn_##a_name { \
    using namespace std;\
    a_code; \
  } \
  namespace { fcf::Parallel::Registrator __fcf_parallel_unit_reg_##a_name(#a_name, #a_options, #a_code, fcf_parallel_unit_nn_##a_name::FCF_PARALLEL_MAIN);  }


#ifndef FCF_PARALLEL_ADD_EXTENSION
  #define FCF_PARALLEL_ADD_EXTENSION__VARNAME2(a_varName, a_funcName, a_line) a_varName##_##a_funcName##_##a_line
  #define FCF_PARALLEL_ADD_EXTENSION__VARNAME(a_varName, a_funcName, a_line) FCF_PARALLEL_ADD_EXTENSION__VARNAME2(a_varName, a_funcName, a_line)
  #define FCF_PARALLEL_ADD_EXTENSION(a_name, a_extensionType) \
    fcf::Parallel::ExtensionRegistrator<a_extensionType> \
      FCF_PARALLEL_ADD_EXTENSION__VARNAME(extensionRegistrator, a_name, __LINE__)(#a_name);
#endif // #ifndef FCF_MATH_REGISTRY_FUNCTION



#define FCF_PARALLEL_GLOBAL

#endif
