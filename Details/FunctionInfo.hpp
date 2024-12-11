#ifndef ___FCF__PARALLEL__DETAILS__FUNCTION_INFO_HPP___
#define ___FCF__PARALLEL__DETAILS__FUNCTION_INFO_HPP___

#include <tuple>
#include <memory>
#include <vector>
#include "../macro.hpp"

namespace fcf {
  namespace Parallel {
    namespace Details {

      template <typename TFunction>
      class FunctionInfo{
      };

      template <size_t Index, size_t IndexEnd, typename... TypePack>
      struct FunctionInfoFiller {
        void operator()(std::vector< std::shared_ptr<BaseArg> >& a_args){
          typedef typename std::tuple_element<Index, std::tuple<TypePack...> >::type type;
          std::shared_ptr<BaseArg> arg((BaseArg*)new Arg<type>());
          a_args.push_back(arg);
          FunctionInfoFiller<Index + 1, IndexEnd, TypePack...>()(a_args);
        }
      };

      template <size_t IndexEnd, typename... TypePack>
      struct FunctionInfoFiller<0, IndexEnd,  TypePack...> {
        void operator()(std::vector< std::shared_ptr<BaseArg> >& a_args){
          FunctionInfoFiller<1, IndexEnd, TypePack...>()(a_args);
        }
      };

      template <size_t IndexEnd, typename... TypePack>
      struct FunctionInfoFiller<IndexEnd, IndexEnd, TypePack...> {
        void operator()(std::vector< std::shared_ptr<BaseArg> >& a_args){
        }
      };

      template <typename... TArgPack>
      struct FunctionInfo< void (*)(TArgPack...) > {
        inline void operator()(std::vector< std::shared_ptr<BaseArg> >& a_args) {
          FunctionInfoFiller<0, sizeof...(TArgPack), TArgPack...>()(a_args);
        }
      };

    } // Details namespace
  } // Parallel namespace
} // fcf namespace

#endif
