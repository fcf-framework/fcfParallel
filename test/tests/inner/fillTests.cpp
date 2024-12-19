#include <memory>
#include <iostream>
#include "../macro.hpp"
#include "../../../Details/fill.hpp"

namespace FillTest{
  struct Balance {
    unsigned long long threadCount;
    unsigned long long duration;
    unsigned long long pduration;
    size_t             step;
    double             deviation;
    double             attitude;
    double             lattitude;
    unsigned long long minDuration;
    unsigned long long packageSize;

    unsigned long long lastTasks;
    unsigned long long lresult;
    unsigned long long div;

    Balance()
      : threadCount(0)
      , duration(0)
      , pduration(0)
      , step(0)
      , deviation(0)
      , attitude(0)
      , lattitude(0)
      , minDuration(1000*10)
      , packageSize(1000*1000)
      , lastTasks(0)
      , lresult(0)
      , div(0)
    {
    }
  };
  typedef std::shared_ptr<Balance> PBalance;
}

void fillTest(){
  std::cout << "Start fillTest..." << std::endl;
  {
    unsigned long long packageSize = 1000000;
    std::vector<FillTest::PBalance> balances(5);
    for(size_t i = 0; i < balances.size(); ++i) {
      balances[i].reset(new FillTest::Balance());
      balances[i]->threadCount = i ? 1 : 412;
    }
    for(size_t step = 0; step < 5; ++step) {
      unsigned long long minDuration = 0;
      unsigned long long maxDuration = 0;
      for(size_t i = 0; i < balances.size(); ++i) {
        balances[i]->threadCount = i ? 10 : 1000;
        double mpower = i ? 100 : 1000;
        double cpower = mpower;
        unsigned long long duration = (unsigned long long)(step ? ((double)balances[i]->lastTasks / mpower + 1.0/cpower) * 1000 : 0);
        fcf::Parallel::Details::setDuration(
                                    balances[i], 
                                    duration
                                    );
        if (step >= 2) {
          if (!maxDuration){
            unsigned long long d = duration / 50;
            minDuration = duration - d;
            maxDuration = duration + d;
          } else {
            FCF_PARALLEL_TEST(duration > minDuration);
            FCF_PARALLEL_TEST(duration < maxDuration);
          }
        }
      }
      for(size_t i = 0; i < balances.size(); ++i) {
        balances[i]->lastTasks = fcf::Parallel::Details::fill(balances, balances[i], packageSize);
      }
    }

  }
}
