#ifndef ___FCF__PARALLEL__ARG_HPP___
#define ___FCF__PARALLEL__ARG_HPP___

#include "macro.hpp"
#include "type.hpp"

#include <string>
#include <set>
#include <memory>

#include "Details/TupleCall.hpp"

#define FCF_PARALLLEL_MAX_TYPE_LENGTH 64
#define FCF_PARALLEL_MAX_TYPE_COMPATIBLE 9
namespace fcf {
  namespace Parallel {

    enum ParallelSplit{
      PS_NONE,
      PS_FULL,
      PS_PACKAGE,
    };

    class BaseArg;

    struct ArgUpload{
      ArgUpload(bool a_value)
        : value(a_value){}
      inline void operator()(BaseArg* a_parg);
      bool value;
    };

    struct ArgSplit{
      ArgSplit(ParallelSplit a_value)
        : value(a_value){}
      inline void operator()(BaseArg* a_parg);
      ParallelSplit value;
    };

    struct ArgLength{
      ArgLength(size_t a_value)
        : value(a_value){}
      inline void operator()(BaseArg* a_parg);
      size_t value;
    };

    struct ArgSplitSize{
      ArgSplitSize(size_t a_value)
        : value(a_value){}
      inline void operator()(BaseArg* a_parg);
      size_t value;
    };

    struct ArgContainer{
      ArgContainer(size_t a_value)
        : value(a_value){}
      inline void operator()(BaseArg* a_parg);
      size_t value;
    };

    template <typename Ty>
    class Arg;

    namespace Details {
      template <typename Ty>
      struct StaticCompatible {
        StaticCompatible() {
          const char* origArgs[FCF_PARALLEL_MAX_TYPE_COMPATIBLE];
          Arg<Ty>().types(origArgs, typesc);
          for(size_t i = 0; i < typesc; ++i) {
            size_t index = i*2;
            size_t l = strlen(origArgs[i]);
            if (l + 1 >= FCF_PARALLLEL_MAX_TYPE_LENGTH - 1) {
              throw std::runtime_error(std::string() + "The size of the type name may not exceed " + std::to_string(FCF_PARALLLEL_MAX_TYPE_LENGTH-1)); 
            }
            std::strcpy(types[index], origArgs[i]);
            types[index][l] = '*';
            types[index][l+1] = 0;

            index += 1;
            l += 6;
            if (l + 1 >= FCF_PARALLLEL_MAX_TYPE_LENGTH - 1) {
              throw std::runtime_error(std::string() + "The size of the type name may not exceed " + std::to_string(FCF_PARALLLEL_MAX_TYPE_LENGTH-1)); 
            }
            std::strcpy(types[index], "const ");
            std::strcpy(&types[index][6], origArgs[i]);
            types[index][l] = '*';
            types[index][l+1] = 0;
          }
          typesc *= 2;
        }
        size_t typesc;
        char   types[FCF_PARALLEL_MAX_TYPE_COMPATIBLE][FCF_PARALLLEL_MAX_TYPE_LENGTH];
      };
    }

    class BaseArg {
      public:
        BaseArg()
          : pointer(0)
          , size(0)
          , itemSize(0)
          , length(0)
          , container(false)
          , split(PS_NONE)
          , splitSize(1)
          , upload(false) {
        }
        virtual ~BaseArg();
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) = 0;
        void*         pointer;
        size_t        size;
        size_t        itemSize;
        size_t        length;
        bool          container;
        ParallelSplit split;
        size_t        splitSize;
        bool          upload;
    };

    template <typename Ty>
    class Arg : public BaseArg {
      public:
        typedef Ty                       value_type;
        typedef typename Ty::value_type  argument_item_type;
        typedef typename Ty::value_type* argument_type;

        Arg()
          : pvalue(0)
        {
        }

        Arg(const Arg& a_arg)
          : BaseArg(a_arg) {
          if (a_arg.data.get() && a_arg.pvalue == &(*a_arg.data->begin()) ) {
            data = a_arg.data;
            pvalue = &(*data->begin());
          } else {
            pvalue = a_arg.pvalue;
          }
          pointer = &pvalue;
        }


        template <typename... TParamPack>
        Arg(Ty& a_value, bool a_refMode, TParamPack... a_paramPack) {
          container = true;
          size     = sizeof(void*);
          itemSize = sizeof(argument_item_type);
          length   = a_value.size();
          if (a_refMode) {
            pvalue  = &(*a_value.begin());
          } else {
            data.reset(new Ty(a_value));
            pvalue  = &(*data->begin());
          }
          pointer = &pvalue;

          typedef std::tuple<TParamPack ...> tuple_param_type;
          tuple_param_type tupleParams{a_paramPack...};
          Details::tupleCall(tupleParams, this);
        }

        virtual void types(const char* a_args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          static Details::StaticCompatible<argument_item_type> staticCompatible;
          a_dstArgsCount = staticCompatible.typesc;
          for(size_t i = 0; i < a_dstArgsCount; ++i){
            a_args[i] = staticCompatible.types[i];
          }
        }

        argument_type       pvalue;
        std::shared_ptr<Ty> data;
    };

    template <typename Ty>
    class Arg<Ty*> : public BaseArg {
      public:
        typedef Ty* value_type;
        typedef Ty* argument_type;

        Arg(){
        }

        Arg(const Arg& a_arg)
          : BaseArg(a_arg){
          container = true;
          value = a_arg.value;
          pointer  = &value;
        }

        template <typename... TParamPack>
        Arg(Ty* a_value, bool a_refMode, TParamPack... a_paramPack) {
          value    = a_value;
          pointer  = &value;
          itemSize = sizeof(Ty);
          size     = sizeof(void*);
          length   = 0;

          typedef std::tuple<TParamPack ...> tuple_param_type;
          tuple_param_type tupleParams{a_paramPack...};
          Details::tupleCall(tupleParams, this);
        }

        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          static Details::StaticCompatible<Ty> staticCompatible;
          a_dstArgsCount = staticCompatible.typesc;
          for(size_t i = 0; i < a_dstArgsCount; ++i){
            args[i] = staticCompatible.types[i];
          }
        }

        Ty*                 value;
    };


    template <typename Ty>
    class Arg<const Ty*> : public BaseArg {
      public:
        typedef Ty* value_type;
        typedef Ty* argument_type;

        Arg(){
        }

        Arg(const Arg& a_arg)
          : BaseArg(a_arg){
          container = true;
          value = a_arg.value;
          pointer  = &value;
        }

        template <typename... TParamPack>
        Arg(const Ty* a_value, bool a_refMode, TParamPack... a_paramPack) {
          value    = a_value;
          pointer  = &value;
          itemSize = sizeof(Ty);
          size     = sizeof(void*);
          length   = 0;

          typedef std::tuple<TParamPack ...> tuple_param_type;
          tuple_param_type tupleParams{a_paramPack...};
          Details::tupleCall(tupleParams, this);
        }

        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          static Details::StaticCompatible<Ty> staticCompatible;
          a_dstArgsCount = staticCompatible.typesc;
          for(size_t i = 0; i < a_dstArgsCount; ++i){
            args[i] = staticCompatible.types[i];
          }
        }

        const Ty* value;
    };


    template <typename Ty>
    class SimpleArg : public BaseArg {
      public:
        typedef Ty          value_type;
        typedef Ty          argument_type;

        SimpleArg(){
        }

        SimpleArg(const SimpleArg& a_arg)
          : BaseArg(a_arg)
          , value(a_arg.value){
          if (&a_arg.value == a_arg.pointer){
            pointer = &value;
          }
        }

        template <typename... TParamPack>
        SimpleArg(Ty& a_value, bool a_refMode, TParamPack... a_paramPack) {
          size     = sizeof(Ty);
          itemSize = 0;
          length   = 0;
          if (a_refMode) {
            pointer = &a_value;
            value   = a_value;
          } else {
            value   = a_value;
            pointer = &value;
          }

          typedef std::tuple<TParamPack ...> tuple_param_type;
          tuple_param_type tupleParams{a_paramPack...};
          Details::tupleCall(tupleParams, this);
        }

        Ty value;
    };

    template <>
    class Arg<bool> : public SimpleArg<bool> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 1;
          args[0] = "bool";
        }
    };

    template <>
    class Arg<char> : public SimpleArg<char> {
      public:
        using SimpleArg::SimpleArg;

        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "char";
          args[1] = "unsigned char";
        }
    };

    template <>
    class Arg<unsigned char> : public SimpleArg<unsigned char> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "unsigned char";
          args[1] = "char";
        }
    };

    template <>
    class Arg<short> : public SimpleArg<short> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "short";
          args[1] = "unsigned short";
        }
    };

    template <>
    class Arg<unsigned short> : public SimpleArg<unsigned short> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "unsigned short";
          args[1] = "short";
        }
    };

    template <>
    class Arg<int> : public SimpleArg<int> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "int";
          args[1] = "unsigned int";
        }
    };

    template <>
    class Arg<unsigned int> : public SimpleArg<unsigned int> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "unsigned int";
          args[1] = "int";
        }
    };

    template <>
    class Arg<fcf_int64> : public SimpleArg<fcf_int64> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "long long";
          args[1] = "unsigned long long";
        }
    };

    template <>
    class Arg<fcf_uint64> : public SimpleArg<fcf_uint64> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 2;
          args[0] = "unsigned long long";
          args[1] = "long long";
        }
    };

    template <>
    class Arg<size_t> : public SimpleArg<size_t> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 1;
          args[0] = "size_t";
        }
    };

    template <>
    class Arg<float> : public SimpleArg<float> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 1;
          args[0] = "float";
        }
    };


    template <>
    class Arg<double> : public SimpleArg<double> {
      public:
        using SimpleArg::SimpleArg;
        virtual void types(const char* args[FCF_PARALLEL_MAX_TYPE_COMPATIBLE], size_t& a_dstArgsCount) {
          a_dstArgsCount = 1;
          args[0] = "double";
        }
    };


    template <typename Ty>
    struct ArgResolver{
      typedef Ty type;
      typedef Arg<Ty> arg_type;
      static inline Arg<Ty> resolve(const Ty& a_value){
        return Arg<Ty>((Ty&)a_value, false);
      }
    };

    template <typename Ty>
    struct ArgResolver< Arg<Ty> > {
      typedef Ty type;
      typedef Arg<Ty> arg_type;
      static inline Arg<Ty> resolve(const Arg<Ty>& a_value){
        return Arg<Ty>(a_value);
      }
    };


    template <typename Ty, typename... TParamPack>
    Arg<Ty> refArg(Ty& a_value, TParamPack... a_paramPack){
      Arg<Ty> a(a_value, true, a_paramPack...);
      return a;
    }

    template <typename Ty, typename... TParamPack>
    Arg<Ty> valArg(const Ty& a_value, TParamPack... a_paramPack){
      Arg<Ty> a(a_value, false, a_paramPack...);
      return a;
    }

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      BaseArg::~BaseArg() {
      }
    #endif

    void ArgUpload::operator()(BaseArg* a_parg){
      a_parg->upload = value;
    }

    void ArgSplit::operator()(BaseArg* a_parg){
      a_parg->split = value;
    }

    void ArgLength::operator()(BaseArg* a_parg){
      a_parg->length = value;
    }

    void ArgSplitSize::operator()(BaseArg* a_parg){
      a_parg->splitSize = value;
    }

    void ArgContainer::operator()(BaseArg* a_parg){
      a_parg->container = value;
    }

  } // namespace Parallel
} // namesapce fcf

#endif // #ifndef ___FCF__PARALLEL__ARG_HPP___
