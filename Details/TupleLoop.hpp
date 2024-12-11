#ifndef ___FCF__PARALLEL__DETAILS__TUPLE_LOOP_HPP___
#define ___FCF__PARALLEL__DETAILS__TUPLE_LOOP_HPP___

#include <tuple>
#include "../macro.hpp"

namespace fcf {
  namespace Parallel {
    namespace Details {

      template <typename Tuple, size_t Index, size_t IndexEnd>
      struct TupleLoop {
        template<typename TFunction>
        void operator()(Tuple& a_tuple, TFunction a_function){
          a_function(&std::get<Index>(a_tuple), Index);
          TupleLoop<Tuple, Index+1, IndexEnd>()(a_tuple, a_function);
        }
      };

      template <typename Tuple, size_t IndexEnd>
      struct TupleLoop<Tuple, IndexEnd, IndexEnd>{
        template<typename TFunction>
        void operator()(Tuple& a_tuple, TFunction a_function){
        }
      };

      template <typename TTuple, typename TFunction>
      void tupleLoop(TTuple& a_tuple, TFunction a_function){
        TupleLoop<TTuple, 0, std::tuple_size<TTuple>::value >()(a_tuple, a_function);
      }

    } // Details namespace
  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__DETAILS__TUPLE_LOOP_HPP___
