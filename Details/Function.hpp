#ifndef ___FCF__PARALLEL__DETAILS__FUNCTION_HPP___
#define ___FCF__PARALLEL__DETAILS__FUNCTION_HPP___

#include "../macro.hpp"

namespace fcf {
  #ifndef FCF_TNOP_STRUCT_DEFINED
  #define FCF_TNOP_STRUCT_DEFINED
    struct TNOP {};
  #endif
} // fcf namespace

namespace fcf {
  namespace Parallel {
    namespace Details {
      #define FCF_PARALLEL_MAX_FUNCTION_ARGS 20
      
      template <typename Ty> struct IsNop       {  enum { value = 0 }; };
      template <>            struct IsNop<TNOP> {  enum { value = 1 }; };

      class Function;

      template <typename Ty>
      struct UnRef {
        typedef Ty type;
      };

      template <typename Ty>
      struct UnRef<Ty&> {
        typedef Ty type;
      };

      template <typename TCallback>
      struct FunctionCaller;

      struct Function {
          inline Function()
            : w(0)
            , cb(0)
          {}

          template <typename TCallback, typename TArg1 = TNOP, typename TArg2 = TNOP, typename TArg3 = TNOP, typename TArg4 = TNOP, typename TArg5 = TNOP,
                                        typename TArg6 = TNOP, typename TArg7 = TNOP, typename TArg8 = TNOP, typename TArg9 = TNOP, typename TArg10 = TNOP,
                                        typename TArg11 = TNOP, typename TArg12 = TNOP, typename TArg13 = TNOP, typename TArg14 = TNOP, typename TArg15 = TNOP,
                                        typename TArg16 = TNOP, typename TArg17 = TNOP, typename TArg18 = TNOP, typename TArg19 = TNOP, typename TArg20 = TNOP
                                        >
          Function(TCallback a_callback,
                   TArg1* a_parg1 = 0, TArg2* a_parg2 = 0, TArg3* a_parg3 = 0, TArg4* a_parg4 = 0, TArg5* a_parg5 = 0,
                   TArg6* a_parg6 = 0, TArg7* a_parg7 = 0, TArg8* a_parg8 = 0, TArg9* a_parg9 = 0, TArg10* a_parg10 = 0,
                   TArg11* a_parg11 = 0, TArg12* a_parg12 = 0, TArg13* a_parg13 = 0, TArg14* a_parg14 = 0, TArg15* a_parg15 = 0,
                   TArg16* a_parg16 = 0, TArg17* a_parg17 = 0, TArg18* a_parg18 = 0, TArg19* a_parg19 = 0, TArg20* a_parg20 = 0
              )
          {
            if (!IsNop<TArg1>::value)  { args[0] =  a_parg1; }
            if (!IsNop<TArg2>::value)  { args[1] =  a_parg2; }
            if (!IsNop<TArg3>::value)  { args[2] =  a_parg3; }
            if (!IsNop<TArg4>::value)  { args[3] =  a_parg4; }
            if (!IsNop<TArg5>::value)  { args[4] =  a_parg5; }
            if (!IsNop<TArg6>::value)  { args[5] =  a_parg6; }
            if (!IsNop<TArg7>::value)  { args[6] =  a_parg7; }
            if (!IsNop<TArg8>::value)  { args[7] =  a_parg8; }
            if (!IsNop<TArg9>::value)  { args[8] =  a_parg9; }
            if (!IsNop<TArg10>::value) { args[9] =  a_parg10; }
            if (!IsNop<TArg11>::value) { args[10] =  a_parg11; }
            if (!IsNop<TArg12>::value) { args[11] =  a_parg12; }
            if (!IsNop<TArg13>::value) { args[12] =  a_parg13; }
            if (!IsNop<TArg14>::value) { args[13] =  a_parg14; }
            if (!IsNop<TArg15>::value) { args[14] =  a_parg15; }
            if (!IsNop<TArg16>::value) { args[15] =  a_parg16; }
            if (!IsNop<TArg17>::value) { args[16] =  a_parg17; }
            if (!IsNop<TArg18>::value) { args[17] =  a_parg18; }
            if (!IsNop<TArg19>::value) { args[18] =  a_parg19; }
            if (!IsNop<TArg20>::value) { args[19] =  a_parg20; }
            w = (void*)FunctionCaller<TCallback>::call;
            cb = (void*)a_callback;
          }

          template <typename TArg1 = TNOP, typename TArg2 = TNOP, typename TArg3 = TNOP, typename TArg4 = TNOP, typename TArg5 = TNOP,
                    typename TArg6 = TNOP, typename TArg7 = TNOP, typename TArg8 = TNOP, typename TArg9 = TNOP, typename TArg10 = TNOP,
                    typename TArg11 = TNOP, typename TArg12 = TNOP, typename TArg13 = TNOP, typename TArg14 = TNOP, typename TArg15 = TNOP,
                    typename TArg16 = TNOP, typename TArg17 = TNOP, typename TArg18 = TNOP, typename TArg19 = TNOP, typename TArg20 = TNOP
                    >
          void operator()(TArg1* a_parg1 = 0, TArg2* a_parg2 = 0, TArg3* a_parg3 = 0, TArg4* a_parg4 = 0, TArg5* a_parg5 = 0,
                          TArg6* a_parg6 = 0, TArg7* a_parg7 = 0, TArg8* a_parg8 = 0, TArg9* a_parg9 = 0, TArg10* a_parg10 = 0,
                          TArg11* a_parg11 = 0, TArg12* a_parg12 = 0, TArg13* a_parg13 = 0, TArg14* a_parg14 = 0, TArg15* a_parg15 = 0,
                          TArg16* a_parg16 = 0, TArg17* a_parg17 = 0, TArg18* a_parg18 = 0, TArg19* a_parg19 = 0, TArg20* a_parg20 = 0) {
            typedef void (*TWrapper)(Function*);
            if (!IsNop<TArg1>::value)  { args[0] =  a_parg1; }
            if (!IsNop<TArg2>::value)  { args[1] =  a_parg2; }
            if (!IsNop<TArg3>::value)  { args[2] =  a_parg3; }
            if (!IsNop<TArg4>::value)  { args[3] =  a_parg4; }
            if (!IsNop<TArg5>::value)  { args[4] =  a_parg5; }
            if (!IsNop<TArg6>::value)  { args[5] =  a_parg6; }
            if (!IsNop<TArg7>::value)  { args[6] =  a_parg7; }
            if (!IsNop<TArg8>::value)  { args[7] =  a_parg8; }
            if (!IsNop<TArg9>::value)  { args[8] =  a_parg9; }
            if (!IsNop<TArg10>::value) { args[9] =  a_parg10; }
            if (!IsNop<TArg11>::value) { args[10] =  a_parg11; }
            if (!IsNop<TArg12>::value) { args[11] =  a_parg12; }
            if (!IsNop<TArg13>::value) { args[12] =  a_parg13; }
            if (!IsNop<TArg14>::value) { args[13] =  a_parg14; }
            if (!IsNop<TArg15>::value) { args[14] =  a_parg15; }
            if (!IsNop<TArg16>::value) { args[15] =  a_parg16; }
            if (!IsNop<TArg17>::value) { args[16] =  a_parg17; }
            if (!IsNop<TArg18>::value) { args[17] =  a_parg18; }
            if (!IsNop<TArg19>::value) { args[18] =  a_parg19; }
            if (!IsNop<TArg20>::value) { args[19] =  a_parg20; }
            ((TWrapper)w)(this);
          }

          void* args[20];
          void* w;
          void* cb;
      };

      template <>
      struct FunctionCaller<void (*)()> {
        typedef void (*callback_type)();
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)();
        }
      };

      template <typename TArg1>
      struct FunctionCaller<void (*)(TArg1)> {
        typedef void (*callback_type)(TArg1);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0]);
        }
      };

      template <typename TArg1,  typename TArg2>
      struct FunctionCaller<void (*)(TArg1,  TArg2)> {
        typedef void (*callback_type)(TArg1,  TArg2);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10]
                                          );
        }
      };


      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14, typename TArg15>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14, TArg15)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14, TArg15);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13],
                                          *(typename UnRef<TArg15>::type*)a_function->args[14]
                                          );
        }
      };


      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14, typename TArg15, typename TArg16>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14, TArg15, TArg16)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14, TArg15, TArg16);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13],
                                          *(typename UnRef<TArg15>::type*)a_function->args[14],
                                          *(typename UnRef<TArg16>::type*)a_function->args[15]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14, typename TArg15, typename TArg16, typename TArg17>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13],
                                          *(typename UnRef<TArg15>::type*)a_function->args[14],
                                          *(typename UnRef<TArg16>::type*)a_function->args[15],
                                          *(typename UnRef<TArg17>::type*)a_function->args[16]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14, typename TArg15, typename TArg16, typename TArg17, typename TArg18>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17, TArg18)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17, TArg18);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13],
                                          *(typename UnRef<TArg15>::type*)a_function->args[14],
                                          *(typename UnRef<TArg16>::type*)a_function->args[15],
                                          *(typename UnRef<TArg17>::type*)a_function->args[16],
                                          *(typename UnRef<TArg18>::type*)a_function->args[17]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14, typename TArg15, typename TArg16, typename TArg17, typename TArg18, typename TArg19>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17, TArg18, TArg19)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17, TArg18, TArg19);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13],
                                          *(typename UnRef<TArg15>::type*)a_function->args[14],
                                          *(typename UnRef<TArg16>::type*)a_function->args[15],
                                          *(typename UnRef<TArg17>::type*)a_function->args[16],
                                          *(typename UnRef<TArg18>::type*)a_function->args[17],
                                          *(typename UnRef<TArg19>::type*)a_function->args[18]
                                          );
        }
      };

      template <typename TArg1,  typename TArg2,  typename TArg3,  typename TArg4,  typename TArg5,  typename TArg6,  typename TArg7,  typename TArg8,  typename TArg9,  typename TArg10,
                typename TArg11, typename TArg12, typename TArg13, typename TArg14, typename TArg15, typename TArg16, typename TArg17, typename TArg18, typename TArg19, typename TArg20>
      struct FunctionCaller<void (*)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                     TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17, TArg18, TArg19, TArg20)> {
        typedef void (*callback_type)(TArg1,  TArg2,  TArg3,  TArg4,  TArg5,  TArg6,  TArg7,  TArg8,  TArg9,  TArg10,
                                      TArg11, TArg12, TArg13, TArg14, TArg15, TArg16, TArg17, TArg18, TArg19, TArg20);
        static void call(Function* a_function) {
          ((callback_type)a_function->cb)(*(typename UnRef<TArg1>::type*)a_function->args[0],
                                          *(typename UnRef<TArg2>::type*)a_function->args[1],
                                          *(typename UnRef<TArg3>::type*)a_function->args[2],
                                          *(typename UnRef<TArg4>::type*)a_function->args[3],
                                          *(typename UnRef<TArg5>::type*)a_function->args[4],
                                          *(typename UnRef<TArg6>::type*)a_function->args[5],
                                          *(typename UnRef<TArg7>::type*)a_function->args[6],
                                          *(typename UnRef<TArg8>::type*)a_function->args[7],
                                          *(typename UnRef<TArg9>::type*)a_function->args[8],
                                          *(typename UnRef<TArg10>::type*)a_function->args[9],
                                          *(typename UnRef<TArg11>::type*)a_function->args[10],
                                          *(typename UnRef<TArg12>::type*)a_function->args[11],
                                          *(typename UnRef<TArg13>::type*)a_function->args[12],
                                          *(typename UnRef<TArg14>::type*)a_function->args[13],
                                          *(typename UnRef<TArg15>::type*)a_function->args[14],
                                          *(typename UnRef<TArg16>::type*)a_function->args[15],
                                          *(typename UnRef<TArg17>::type*)a_function->args[16],
                                          *(typename UnRef<TArg18>::type*)a_function->args[17],
                                          *(typename UnRef<TArg19>::type*)a_function->args[18],
                                          *(typename UnRef<TArg20>::type*)a_function->args[19]
                                          );
        }
      };

    } // Details namespace
  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__DETAILS__FUNCTION_HPP___

