#ifndef ___FCF_PARALLEL__EXTENSION__FUNCTION__FUNCTION_HPP___
#define ___FCF_PARALLEL__EXTENSION__FUNCTION__FUNCTION_HPP___

#include "../../macro.hpp"
#include "../../Extension.hpp"

namespace fcf {
  namespace Parallel {
    namespace Extensions {
      namespace Function {
        class FCF_PARALLEL_DECL_EXPORT FunctionExtension : public Extension {
          virtual ~FunctionExtension();
          virtual void prepareCode(const fcf::Union& a_options, Extension::PrepareCodeInfo& a_info);
        };

        #ifdef FCF_PARALLEL_IMPLEMENTATION
          FunctionExtension::~FunctionExtension(){
          }
        #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

        #ifdef FCF_PARALLEL_IMPLEMENTATION
          void FunctionExtension::prepareCode(const fcf::Union& a_options, fcf::Parallel::Extension::PrepareCodeInfo& a_info){
            std::string cases;

            std::string engineName = "engine_" + (std::string)a_info.engine->property("name");
            std::string deviceType = "device_type_" + (std::string)a_info.engine->property("devices")[a_info.deviceIndex]["type"];
            const char* spaces[] = {engineName.c_str(), deviceType.c_str()};
            unsigned int funcc = IndexableFunctionRegistrator::getSize();

            for(unsigned int index = 0; index < funcc; ++index ) {
              const IndexableFunctionInfo& sfi = IndexableFunctionRegistrator::getFunctionInfo(index);
              std::vector<IndexableFunctionSpace>::const_iterator it = sfi.getSpace(spaces);
              if (it != sfi.spaces.cend()){
                a_info.functions.push_back(
                  {
                    it->sourceName,
                    it->code,
                  }
                );
                cases += std::string() + "    case " + std::to_string(index) + ": return (void*)" + it->sourceName + ";\n";
              }
            }
            a_info.body +=
              std::string() +
              "\n"
              "void* fcf_get_function(unsigned int a_index) {\n"
              "  switch(a_index) {\n"
              + cases +
              "    default: return (void*)0;\n"
              "  }\n"
              "}\n"
              "\n"
              "#define FCF_GET_FUNCTION(a_index) fcf_get_function(a_index) \n"
              "\n"
              "\n"
              ;
          }
        #endif // #ifdef FCF_MATH_IMPLEMENTATION
      } // Function namespace
    } // Extensions namespace
  } // Parallel namespace
} // fcf namespace

#ifdef FCF_PARALLEL_IMPLEMENTATION
  FCF_PARALLEL_ADD_EXTENSION(fcf_function, fcf::Parallel::Extensions::Function::FunctionExtension);
#endif // #ifdef FCF_MATH_IMPLEMENTATION


#endif // #ifndef ___FCF_PARALLEL__EXTENSION__FUNCTION__FUNCTION_HPP___
