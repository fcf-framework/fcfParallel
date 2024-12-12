#include "../../Details/Distributor.hpp"
#include "../macro.hpp"


namespace {


  void test001(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, void* a_ud) {
    fcf::Union& output = *(fcf::Union*)a_ud;
    for(size_t i = 0; i < a_subtask.size; ++i) {
      unsigned long long index = a_subtask.offset + i;
      if (!output[index].is<fcf::Undefined>()) {
        throw std::runtime_error("Logic error ");
      }
      /*
      for(size_t j = 0; j < 100; ++j) {
        a_output[a_subtask.offset + i] = a_subtask.offset + i;
      }
      */
      output[index] = index;

    }
  }
}

void distributorTest(){

  FCF_PARALLEL_TEST(1 == 1);
  {
    fcf::Parallel::Details::Distributor distributor;
    distributor.addDevice(1, 0, 1, 100*1000);
    distributor.addDevice(2, 0, 1, 100*1000);
    distributor.addDevice(3, 0, 1, 100*1000);
    distributor.addDevice(4, 0, 1, 100*1000);

    size_t tnumber = 100000;

    fcf::Union output(fcf::UT_VECTOR);
    output.ref<fcf::UnionVector>().resize(tnumber);

    fcf::Parallel::Details::Distributor::Call call;
    memset(&call, 0, sizeof(call));
    call.name        = "test001";
    call.count       = tnumber;
    call.split       = false;
    call.packageSize = 1000;
    call.packageTime = 0;
    call.function    = test001;
    call.userData    = &output;
    distributor.call(call);

    std::sort(output.ref<fcf::UnionVector>().begin(), output.ref<fcf::UnionVector>().end());

    fcf::Union uexpected(fcf::UT_VECTOR);
    for(size_t i = 0; i < tnumber; ++i){
      uexpected[i] = i;
    }

    FCF_PARALLEL_TEST(output.equal(uexpected, false, true));
  }
  }
