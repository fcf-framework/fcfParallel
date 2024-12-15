#ifndef ___FCF__PARALLEL__DETAILS__FILL_HPP___
#define ___FCF__PARALLEL__DETAILS__FILL_HPP___

#include <vector>
#include <cmath>
#include <algorithm>
namespace fcf {
  namespace Parallel {
    namespace Details {
      template <typename TPBalance>
      void setDuration(TPBalance& a_curBalance, unsigned long long a_taskDuration, bool a_first = false) {
        double div = a_first ? 10 : a_curBalance->div;
        a_curBalance->duration = a_taskDuration * div;
      }

      inline unsigned long long getDuration(unsigned long long a_packageSize, unsigned long long a_duration, unsigned long long a_minDuration) {
        unsigned long long div;
        if (!a_minDuration || !a_duration || !a_packageSize){
          div = 100;
        } else {
          div = a_duration / a_minDuration;
        }
        if (div < 5) {
          div = 5;
        }
        if (a_packageSize && div >= a_packageSize) {
          return a_packageSize;
        }
        return div;
      }

      template <typename TPBalance>
      unsigned long long fill(std::vector<TPBalance>& a_balances, TPBalance& a_curBalance, unsigned long long a_packageSize) {

        unsigned long long minDurationForDiv = 200*1000;
        unsigned long long durationForDiv = 0;
        for(size_t i = 0; i < a_balances.size(); ++i){
          minDurationForDiv = std::max(minDurationForDiv, a_balances[i]->minDuration);
          durationForDiv    = std::max(durationForDiv, a_balances[i]->duration);
        }
        unsigned long long div = getDuration(a_packageSize, durationForDiv, minDurationForDiv);

        if (a_balances.size() == 1){
          a_curBalance->lresult = a_packageSize / div;
          a_curBalance->div = div;
          return a_curBalance->lresult;
        }

        double asum  = 0;
        unsigned long long sum  = 0;
        unsigned long long qsum  = 0;
        unsigned long long sumc = 0;
        bool               full = true;
        for(size_t i = 0; i < a_balances.size(); ++i){
          if (a_balances[i]->duration && a_curBalance->attitude) {
            ++sumc;
          } else {
            full = false;
          }
          sum  += a_balances[i]->duration;
          qsum += a_balances[i]->duration * a_balances[i]->duration;
          asum += a_balances[i]->attitude;
        }
        if (!sumc) {
          full = false;
        }
        double minsuma = 1.0;
        double maxsuma = 1.2;
        double average = std::sqrt((double)qsum / (double)sumc);
        double attitude = a_curBalance->attitude;
        if (full) {
          a_curBalance->step = (unsigned int)a_curBalance->step + 1;
          if (a_curBalance->step > div / 2 && div / 2 > 5){
            a_curBalance->step = div / 2;
          }
          if (a_curBalance->step < 1){
            a_curBalance->step = 1;
          }
          if (a_curBalance->step > 20){
            a_curBalance->step = 20;
          }
          if (a_curBalance->step == 1) {
            double casum = 0.0;
            unsigned long long md = a_balances[0]->duration;
            for(size_t i = 0; i < a_balances.size(); ++i) {
              TPBalance cbl = a_balances[i];
              casum += cbl->lattitude * md / cbl->duration;
            }
            attitude = a_curBalance->lattitude * md / a_curBalance->duration;
            attitude /= casum;
          } else {
            double nattitude = average * a_curBalance->attitude / a_curBalance->duration;
            double d         = nattitude - a_curBalance->attitude;
            double k = 1.0;
            if (
                (asum < minsuma && d < 0) ||
                (asum > maxsuma && d > 0)
                ) {
              k = 0.1;
            }
            attitude += d * (1.0 / a_curBalance->step) * k;
            attitude = std::max(attitude, 0.000001);
          }
          a_curBalance->lattitude = a_curBalance->attitude;
          a_curBalance->attitude = attitude;
        } else {
          sum = 0;
          for(size_t i = 0; i < a_balances.size(); ++i){
            sum += a_balances[i]->threadCount;
          }
          a_curBalance->attitude = (double)a_curBalance->threadCount / (double)sum;
          a_curBalance->lattitude = a_curBalance->attitude;
          attitude = a_curBalance->attitude;
        }
        double dresult  = a_packageSize * attitude / div;
        a_curBalance->lresult = std::min(std::max((unsigned long long)dresult + (std::remainder(dresult, 1) ? 1 : 0), (unsigned long long)1), (unsigned long long)a_packageSize);

        a_curBalance->div = div;
        return a_curBalance->lresult;
      }

    } //  Details namespace
  } // Parallel namespace
} // fcf namespace

#endif
