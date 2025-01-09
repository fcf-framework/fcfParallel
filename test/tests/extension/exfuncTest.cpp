#include "../../../Executor.hpp"
#include "../macro.hpp"
#include <iostream>


float mul2(float a_value){
  return a_value*2;
}


namespace Test {
  class TestFunctionExtension : public fcf::Parallel::Extension {
    virtual ~TestFunctionExtension(){
    }
    virtual void prepareCode(const fcf::Union& a_options, fcf::Parallel::Extension::PrepareCodeInfo& a_info) {
      /*
      if (a_info.declFunctions.find("mul2") != a_info.declFunctions.end()) {
        return;
      }
      */
      a_info.functions.push_back({
          "mul2",
          "float mul2(float a_value){ \n"
          "  return a_value*2; "
          "}"
          "\n\n"
          });
    }
  };
}

FCF_PARALLEL_ADD_EXTENSION(exfunction_extension, Test::TestFunctionExtension);

FCF_PARALLEL_UNIT(
    test_unit_exufunction_001,
    {
      extensions: [
        { name: "exfunction_extension" }
      ]
    },
    void FCF_PARALLEL_MAIN(const FCFParallelTask* a_task, float a_index, FCF_PARALLEL_GLOBAL float* a_array){
     a_array[a_task->lowIndex] = mul2(a_index);
    }
);


void exfuncTest() {
  std::cout << "Start exfuncTest..." << std::endl;
  FCF_PARALLEL_ADD_EXTENSION(exfunction_extension, Test::TestFunctionExtension);
  {
    fcf::Parallel::Executor executor;
    executor.enableEngines(false);
    executor.enableEngines(true, "cpu", 1);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_exufunction_001";
    call.size = 1000;
    std::vector<float> data(call.size);
    executor(call,
             (float)7.0,
             fcf::Parallel::refArg(data,
                                  fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL),
                                  fcf::Parallel::ArgUpload(true)
                                  )
            );
    for(float& value : data){
      FCF_PARALLEL_TEST(value == 14);
    }
  }
  {
    fcf::Parallel::Executor executor;
    executor.enableEngines(false);
    executor.enableEngines(true, "gpu", 1);
    executor.initialize();
    fcf::Parallel::Call call;
    call.name = "test_unit_exufunction_001";
    call.size = 1000;
    std::vector<float> data(call.size);
    executor(call,
             (float)7.0,
             fcf::Parallel::refArg(data,
                                  fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL),
                                  fcf::Parallel::ArgUpload(true)
                                  )
            );
    for(float& value : data){
      FCF_PARALLEL_TEST(value == 14);
    }
  }

}
