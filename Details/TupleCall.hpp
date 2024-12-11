#ifndef ___FCF__PARALLEL__DETAILS__TUPLE_CALL_HPP___
#define ___FCF__PARALLEL__DETAILS__TUPLE_CALL_HPP___

#include <tuple>
#include "../macro.hpp"

namespace fcf {
  namespace Parallel {
    namespace Details {

      template <typename Tuple, size_t Index, size_t IndexEnd>
      struct TupleCall {
        template<typename... TArgPack>
        void operator()(Tuple& a_tuple, TArgPack... a_argPack){
          std::get<Index>(a_tuple)(a_argPack...);
          TupleCall<Tuple, Index+1, IndexEnd>()(a_tuple, a_argPack...);
        }
      };

      template <typename Tuple, size_t IndexEnd>
      struct TupleCall<Tuple, IndexEnd, IndexEnd>{
        template<typename... TArgPack>
        void operator()(Tuple& a_tuple, TArgPack... a_argPack){
        }
      };

      template<typename Tuple, typename... TArgPack>
      void tupleCall(Tuple& a_tuple, TArgPack... a_argPack){
        TupleCall<Tuple, 0, std::tuple_size<Tuple>::value>()(a_tuple, a_argPack...);
      }

    } // Details namespace
  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__DETAILS__TUPLE_CALL_HPP___
