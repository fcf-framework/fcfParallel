#include "../../Details/Distributor.hpp"
#include "../macro.hpp"
#include <set>

namespace {


  void test001(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, void* a_ud) {
    fcf::Union& output = *(fcf::Union*)a_ud;
    for(size_t i = 0; i < a_subtask.size; ++i) {
      unsigned long long index = a_subtask.offset + i;
      if (!output[index].is<fcf::Undefined>()) {
        throw std::runtime_error("Logic error ");
      }
      output[index] = index;
    }
  }

  void test_ignore_001(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, void* a_ud) {
    fcf::Union& output = *(fcf::Union*)a_ud;
    for(size_t i = 0; i < a_subtask.size; ++i) {
      unsigned long long index = a_subtask.offset + i;
      if (!output[index].is<fcf::Undefined>()) {
        throw std::runtime_error("Logic error ");
      }
      output[index] = std::string() + std::to_string(a_subtask.deviceIndex) + ":"+ std::to_string(a_subtask.deviceSubIndex);
    }
  }
}

void distributorTest(){
  std::cout << "Start distributorTest..." << std::endl;
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
    call.name        = "test001";
    call.count       = tnumber;
    call.split       = false;
    call.packageSize = 1000;
    call.packageTime = 0;
    call.function    = test001;
    call.userData    = &output;

    fcf::Parallel::Details::Distributor::DeviceIndex di;
    di.engineIndex = 2;
    di.deviceIndex = 0;
    call.ignoreDevice.push_back(di);

    distributor.call(call);

    std::sort(output.ref<fcf::UnionVector>().begin(), output.ref<fcf::UnionVector>().end());

    fcf::Union uexpected(fcf::UT_VECTOR);
    for(size_t i = 0; i < tnumber; ++i){
      uexpected[i] = i;
    }

    FCF_PARALLEL_TEST(output.equal(uexpected, false, true));
  }
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
    call.name        = "test_ignore_001";
    call.count       = tnumber;
    call.split       = false;
    call.packageSize = 1000;
    call.packageTime = 0;
    call.function    = test_ignore_001;
    call.userData    = &output;

    fcf::Parallel::Details::Distributor::DeviceIndex di;
    di.engineIndex = 2;
    di.deviceIndex = 0;
    call.ignoreDevice.push_back(di);

    distributor.call(call);

    std::sort(output.ref<fcf::UnionVector>().begin(), output.ref<fcf::UnionVector>().end());

    size_t                counter = 0;
    std::set<std::string> items;
    for(fcf::Union& u : output){
      if (items.find((std::string)u) == items.end()) {
        items.insert((std::string)u);
        ++counter;
        FCF_PARALLEL_TEST(u == "1:0" || u == "3:0" || u == "4:0");
      }
    }
    FCF_PARALLEL_TEST(counter == 3);
  }
}
