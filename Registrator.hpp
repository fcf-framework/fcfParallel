#ifndef ___FCF__PARALLEL__DETAILS__REGISTRATOR_HPP___
#define ___FCF__PARALLEL__DETAILS__REGISTRATOR_HPP___
#include <stdexcept>
#include <map>
#include "macro.hpp"
#include "type.hpp"
#include "Unit.hpp"

namespace fcf {
  namespace Parallel {

      struct Units {
        typedef std::map<std::string, PUnit> units_type;
        units_type units;
      };

      namespace Details {
        FCF_PARALLEL_DELC_EXTERN FCF_PARALLEL_DECL_EXPORT Units __fcf_parallel_units;
      } // Details namespace

      struct FCF_PARALLEL_DECL_EXPORT Registrator {
        inline Registrator(){}

        template<typename TFunction>
        Registrator(const char* a_name, const char* a_code, TFunction a_func) {
          add(a_name, a_code, a_func);
        }

        template<typename TFunction>
        void add(const char* a_name, const char* a_code, TFunction a_func) {
          Details::__fcf_parallel_units.units[a_name] = PUnit(new Unit{a_name, a_code, a_func});
        }

        PUnit get(const char* a_name);
      };

      #ifdef FCF_PARALLEL_IMPLEMENTATION
        PUnit Registrator::get(const char* a_name){
          Units::units_type::iterator it = Details::__fcf_parallel_units.units.find(a_name);
          if (it == Details::__fcf_parallel_units.units.end()){
            throw std::runtime_error("Unit name not found");
          }
          return it->second;
        }
      #endif



  } // Parallel namespace
} // fcf namespace

#endif
