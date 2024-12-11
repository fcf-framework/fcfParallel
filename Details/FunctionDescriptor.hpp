#ifndef ___FCF__PARALLEL__DETAILS__FUNCTION_DESCRIPTOR_HPP___
#define ___FCF__PARALLEL__DETAILS__FUNCTION_DESCRIPTOR_HPP___

#include <string>
#include <regex>
#include "../macro.hpp"

namespace fcf {
  namespace Parallel {
    namespace Details {

      struct FunctionDescriptor {
        FunctionDescriptor(const std::string& a_source, const char* a_functionName, bool a_ignoreFirst);
        std::string trim(const std::string& a_source);
        std::string declArgs;
        std::string args;
      };

      #ifdef FCF_PARALLEL_IMPLEMENTATION
        FunctionDescriptor::FunctionDescriptor(const std::string& a_source, const char* a_functionName, bool a_ignoreFirst){
          std::smatch mainFunctionMatch;
          std::string sreg = std::string() + "void[\\s]+" + a_functionName + "[\\s]*\\(([^\\)]*)\\)";
          bool res = std::regex_search(a_source, mainFunctionMatch, std::regex(sreg));
          if (!res || mainFunctionMatch.size()<2){
            throw std::runtime_error("Failed find FCF_PARALLEL_MAIN function");
          }
          std::string fullFunctionArgs = trim(mainFunctionMatch[1]);

          if (a_ignoreFirst) {
            if (fullFunctionArgs.empty()){
              throw std::runtime_error("Function FCF_PARALLEL_MAIN does not have arguments");
            }
            std::string::size_type posFirstArgEnd = fullFunctionArgs.find(",");
            if (posFirstArgEnd == std::string::npos){
              declArgs = "";
            } else {
              declArgs = trim(fullFunctionArgs.substr(posFirstArgEnd + 1));
            }
          } else {
            declArgs = fullFunctionArgs;
          }

          std::string::size_type pos = 0;
          if (declArgs.length()) {
            while(pos != std::string::npos) {
              std::string::size_type npos = declArgs.find(",", pos);
              std::string::size_type i;
              std::string::size_type al;
              std::string::size_type ab;
              if (!npos){
                throw std::runtime_error("Failed FCF_PARALLEL_MAIN declaration");
              }
              if (npos == std::string::npos) {
                i = declArgs.length()-1;
                al = i;
              } else {
                al = npos;
                i = npos - 1;
              }
              ab = pos;
              do {
                if ((unsigned char)declArgs[i] <= (unsigned char)' '){
                  ab = i;
                  --i;
                } else {
                  break;
                }
              } while(i > pos);
              if (i == pos) {
                break;
              }
              bool isArr = false;
              bool isVar = false;
              do {
                if (!isVar && !isArr && declArgs[i] == ']')  {
                  isArr = true;
                  ab = i;
                  --i;
                } else if (!isVar && isArr)  {
                  if (declArgs[i] == '[') {
                    isArr = false;
                  }
                  ab = i;
                  --i;
                  while ((unsigned char)declArgs[i] <= (unsigned char)' ' && i > pos){
                    ab = i;
                    --i;
                  }
                } else if ((unsigned char)declArgs[i] > (unsigned char)' '){
                  isVar = true;
                  ab = i;
                  --i;
                } else {
                  break;
                }
              } while(i > pos);
              if (!args.empty()) {
                args += ' ';
              }
              args += trim(declArgs.substr(ab, al - ab + 1));
              pos = npos == std::string::npos ? std::string::npos : npos + 1;
            }
          }
        }
      #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

      #ifdef FCF_PARALLEL_IMPLEMENTATION
        std::string FunctionDescriptor::trim(const std::string& a_source){
          if (a_source.empty()){
            return std::string();
          }
          size_t left  = 0;
          size_t right = a_source.length()-1;
          while((unsigned char)a_source[left] <= (unsigned char)' ') {
            if (left == right){
              return std::string();
            }
            ++left;
          }
          while((unsigned char)a_source[right] <= (unsigned char)' ') {
            --right;
          }
          return std::string(&a_source[0] + left, &a_source[0] + right + 1);
        }
      #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    } // Details namespace
  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__DETAILS__FUNCTION_HPP___

