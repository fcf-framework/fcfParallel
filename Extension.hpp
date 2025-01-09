#ifndef ___FCF__PARALLEL__EXTENSION_HPP___
#define ___FCF__PARALLEL__EXTENSION_HPP___

#include <memory>
#include <map>
#include <set>
#include "macro.hpp"
#include "include.hpp"

namespace fcf {
  namespace Parallel {

    class Extension;
    class BaseEngine;

    namespace Details {
      FCF_PARALLEL_DELC_EXTERN FCF_PARALLEL_DECL_EXPORT std::map<std::string, Extension*(*)()> __fcf_parallel_extensions;

      template <typename Ty>
      Extension* createExtension(){
        return new Ty();
      }
    } // Details namespace

    class FCF_PARALLEL_DECL_EXPORT Extension {
      public:
        virtual ~Extension();

        struct PrepareCodeInfo {
          BaseEngine*                                      engine;
          std::list< std::pair<std::string, std::string> > functions;
          std::string                                      prefix;
          std::string                                      body;
          std::string                                      suffix;
          std::string                                      code;
        };

        virtual void prepareCode(const Union& a_options, PrepareCodeInfo& a_info) = 0;

        template <typename Ty>
        static void add(const std::string& a_name) {
          Details::__fcf_parallel_extensions[a_name] = Details::createExtension<Ty>;
        }

        static std::shared_ptr<Extension> create(const std::string& a_name);
    };

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      Extension::~Extension(){
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      std::shared_ptr<Extension> Extension::create(const std::string& a_name){
        auto it = Details::__fcf_parallel_extensions.find(a_name);
        if (it == Details::__fcf_parallel_extensions.end()) {
          throw std::runtime_error(std::string() + "Extension with the name '" + a_name + "' is not available");
        }
        return std::shared_ptr<Extension>(it->second());
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    struct ExtensionInfo {
      std::shared_ptr<Extension> extension;
      Union                      options;
    };

    template <typename Ty>
    struct ExtensionRegistrator {
      ExtensionRegistrator(const char* a_name) {
        Extension::add<Ty>(a_name);
      }
    };

  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__EXTENSION_HPP___

