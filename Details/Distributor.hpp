#ifndef ___FCF__PARALLEL__DETAILS__DISTRIBUTOR_HPP___
#define ___FCF__PARALLEL__DETAILS__DISTRIBUTOR_HPP___
#include <stdexcept>
#include <cmath>
#include <memory>
#include <cstring>
#include <atomic>
#include <string>
#include <map>
#include <list>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "../include.hpp"
#include "../macro.hpp"
#include "../Call.hpp"
#include "fill.hpp"
#include "Function.hpp"
#include "../macro.hpp"


namespace fcf {
  namespace Parallel {
    namespace Details {
      class FCF_PARALLEL_DECL_EXPORT Distributor {
          struct Device;
          typedef std::shared_ptr<Device> PDevice;
          struct Balance;
          typedef std::shared_ptr<Balance> PBalance;
          struct ulock;

        public:
          struct SubTask {
            unsigned long long offset;
            unsigned long long size;
            size_t             deviceIndex;
            size_t             deviceSubIndex;
          };

          struct DeviceIndex{
            size_t engineIndex;
            size_t deviceIndex;
          };

          struct Call {
            const char*             name;
            unsigned long long      count;
            bool                    split;
            Union*                  state;
            unsigned long long      packageSize;
            unsigned long long      packageTime;
            void                    (*function)(const SubTask&, void*);
            void*                   userData;
            std::string             lastError;
            std::list<DeviceIndex>  ignoreDevice; // Used for the first time at the first call
                                                  // for a given name and used for the rest of the time¤
            inline Call()
              : name(0)
              , count(0)
              , split(false)
              , state(0)
              , packageSize(0)
              , packageTime(0)
              , function(0)
              , userData(0)
            {
            }
          };

          Distributor();
          ~Distributor();
          void stop();
          void addDevice(size_t a_index, size_t a_subindex, unsigned long long a_threadCount, unsigned long long a_minDuraion);
          void call(const Call& a_call);

        private:
          struct Task {
            std::string                     name;
            void                            (*action)(const SubTask&, void*);
            void*                           userData;
            std::atomic<unsigned long long> offset;
            std::atomic<unsigned long long> completeOffset;
            unsigned long long              readyOffsetEnd;
            unsigned long long              size;
            unsigned long long              packageSize;
            bool                            split;
            Union*                          state;
            std::string                     error;
            bool                            errorFlag;

            inline Task()
              : action(0)
              , offset(0)
              , completeOffset(0)
              , readyOffsetEnd(0)
              , size(0)
              , packageSize(0)
              , split(false)
              , state(0)
              , errorFlag(false)  {
            }
          };
          typedef std::shared_ptr<Task> PTask;

          struct Balance {
            double             attitude;
            double             lattitude;
            double             deviation;
            unsigned long long duration;
            unsigned long long pduration;
            unsigned long long packageSize;
            unsigned long long threadCount;
            unsigned int       step;
            unsigned long long minDuration;
            unsigned long long lresult;
            unsigned long long div;
            bool               ignore;
            inline Balance()
              : attitude(0)
              , lattitude(0)
              , deviation(0)
              , duration(0)
              , pduration(0)
              , packageSize(0)
              , threadCount(0)
              , step(0)
              , minDuration(0)
              , lresult(0)
              , div(0)
              , ignore(false)
            {
            }
          };

          struct Device {
            size_t                          index;
            size_t                          subindex;
            bool                            active;
            unsigned long long              threadCount;
            unsigned long long              minDuration;
            std::thread                     thread;
            std::map<std::string, PBalance> balance;
          };

          class Unlock {
            public:
              inline Unlock(std::unique_lock<std::mutex>& a_lock)
                : _lock(a_lock) {
                a_lock.unlock();
              }
              inline ~Unlock(){
                _lock.lock();
              }
            private:
              std::unique_lock<std::mutex>& _lock;
          };

          class Lock {
            public:
              inline Lock(std::unique_lock<std::mutex>& a_lock)
                : _lock(a_lock) {
                a_lock.lock();
              }
              inline ~Lock(){
                _lock.unlock();
              }
            private:
              std::unique_lock<std::mutex>& _lock;
          };

          inline void        _setStat(PTask& a_task);
          void               _handler(PDevice a_pdevice);
          void               _handlerImpl(PDevice a_pdevice, std::unique_lock<std::mutex>& a_lock);
          void               _balance(Balance& a_balance, unsigned long long a_duration);

          bool                    _stop;
          std::list<PDevice>      _devices;
          PTask                   _task;
          std::mutex              _mutex;
          std::condition_variable _condition;
          std::chrono::time_point<std::chrono::high_resolution_clock> _startPack;
      };

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      Distributor::Distributor()
        : _stop(false) {
      }
      #endif

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      Distributor::~Distributor() {
        stop();
      }
      #endif

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      void Distributor::stop() {
        std::list<PDevice> devices;
        {
          std::unique_lock<std::mutex> lock(_mutex);
          _stop = true;
          devices = _devices;
          _condition.notify_all();
        }
        for(std::list<PDevice>::iterator it = devices.begin(); it != devices.end(); ++it) {
          (*it)->thread.join();
        }
      }
      #endif

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      void Distributor::addDevice(size_t a_index, size_t a_subIndex, unsigned long long a_threadCount, unsigned long long a_minDuraion) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_stop) {
          throw std::runtime_error("The object is stopped");
        }
        PDevice pdevice(new Device{a_index, a_subIndex, false, a_threadCount, a_minDuraion});
        pdevice->thread = std::thread(&Distributor::_handler, this, pdevice);
        _devices.push_back(pdevice);
      }
      #endif

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      void Distributor::call(const Call& a_call) {
        {
          std::unique_lock<std::mutex> lock(_mutex);
          if (_stop) {
            throw std::runtime_error("The object is stopped");
          }
          while(_task.get() && !_stop){
            _condition.wait(lock);
          }
          if (_stop) {
            throw std::runtime_error("The object is stopped");
          }
          PTask task(new Task);
          task->action      = a_call.function;
          task->userData    = a_call.userData;
          task->size        = a_call.count;
          task->split       = a_call.split;
          task->state       = a_call.state;
          task->packageSize = a_call.packageSize;
          task->readyOffsetEnd = task->packageSize;
          task->name = a_call.name ? a_call.name : "";

          if (task->state && !task->state->is(UT_MAP)){
            task->state->ref<fcf::UnionMap>();
          }

          _task = task;

          size_t devCount = 0;
          for(std::list<PDevice>::iterator it = _devices.begin(); it != _devices.end(); ++it){
            if ((*it)->balance.find(task->name) == (*it)->balance.end()) {
              PBalance balance(new Balance());
              balance->threadCount = (*it)->threadCount;
              balance->minDuration = (*it)->minDuration;
              (*it)->balance.insert(std::pair<std::string, PBalance>(task->name, balance));

              for(std::list<DeviceIndex>::const_iterator ignoreIt = a_call.ignoreDevice.begin(); ignoreIt != a_call.ignoreDevice.end(); ++ignoreIt){
                if (ignoreIt->engineIndex == (*it)->index && ignoreIt->deviceIndex == (*it)->subindex) {
                  balance->ignore = true;
                }
              }
            }
            PBalance balance = (*it)->balance[task->name];
            if (balance->packageSize != a_call.packageSize && balance->duration) {
              balance->duration = (unsigned long long)((double)(balance->duration * a_call.packageSize) / (double)balance->packageSize);
              balance->packageSize = a_call.packageSize;
            }
            if (!balance->ignore){
              ++devCount;
            }
          }

          if (!devCount){
            std::string exceptionMessage;
            exceptionMessage = "There are no available devices.";
            if (!a_call.lastError.empty()) {
              exceptionMessage += " Last device error: ";
              exceptionMessage += a_call.lastError;
            }
            throw std::runtime_error(exceptionMessage);
          }

          _startPack = std::chrono::high_resolution_clock::now();
          if (task->state) {
            (*task->state)["maxPackageDuration"] = 0;
            (*task->state)["minPackageDuration"] = 0;
            (*task->state)["packageDuration"] = 0;
          }

          _condition.notify_all();

          while(!task->errorFlag && task->completeOffset < task->size) {
            _condition.wait(lock);
          }

          if (task->errorFlag){
            bool active;
            do {
              active = false;
              for(std::list<PDevice>::iterator it = _devices.begin(); it != _devices.end(); ++it) {
                active |= (*it)->active;
              }
              if (active) {
                _condition.wait(lock);
              }
            } while(active);
          }

          _task.reset();

          if (task->errorFlag) {
            throw std::runtime_error(task->error);
          }

          _setStat(task);
        }

      }
      #endif

      void Distributor::_setStat(PTask& a_task){
        if (a_task->state) {
          unsigned long long packagec = std::max(a_task->size / a_task->packageSize, (unsigned long long)1);
          std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
          unsigned long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - _startPack).count();
          unsigned long long minPackageDuration = (*a_task->state)["minPackageDuration"].isCompatible(fcf::UT_ULONGLONG)
                                                    ? (unsigned long long)(*a_task->state)["minPackageDuration"]
                                                    : 0;
          unsigned long long maxPackageDuration = (*a_task->state)["maxPackageDuration"].isCompatible(fcf::UT_ULONGLONG)
                                                    ? (unsigned long long)(*a_task->state)["maxPackageDuration"]
                                                    : 0;
          unsigned long long packageDuration = (*a_task->state)["packageDuration"].isCompatible(fcf::UT_ULONGLONG)
                                                    ? (unsigned long long)(*a_task->state)["packageDuration"]
                                                    : 0;
          (*a_task->state)["minPackageDuration"] = minPackageDuration 
                                                              ? std::min(duration, minPackageDuration)
                                                              : duration;
          (*a_task->state)["maxPackageDuration"] = std::max(duration, maxPackageDuration);
          if (!packageDuration){
            (*a_task->state)["packageDuration"] = duration;
          } else {
            (*a_task->state)["packageDuration"] = packageDuration - (packageDuration/ packagec) + (duration / packagec);
          }
          _startPack = end;
        }
      }

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      void Distributor::_handler(PDevice a_pdevice) {
        std::unique_lock<std::mutex> lock(_mutex);
        _handlerImpl(a_pdevice, lock);
      }
      #endif

      #ifdef FCF_PARALLEL_IMPLEMENTATION
      void Distributor::_handlerImpl(PDevice a_pdevice, std::unique_lock<std::mutex>& a_lock) {
        typedef std::vector<PBalance>                                            SpecBalances;
        typedef std::shared_ptr<std::vector<PBalance> >                          PSpecBalances;
        typedef std::map<std::string, std::shared_ptr< std::vector<PBalance> > > Balances;
        SubTask  subTask;
        Balances balances;
        while(!_stop) {
          if (_task.get() && !_task->errorFlag && _task->offset < _task->size) {
            PTask              task = _task;
            PBalance           balance = a_pdevice->balance[task->name];
            if (balance->ignore){
              _condition.wait(a_lock);
              continue;
            }
            PSpecBalances      curBalances;
            Balances::iterator curBalancesIterator = balances.find(task->name);
            if (curBalancesIterator == balances.end()) {
              curBalances.reset(new SpecBalances(_devices.size()));
              balances[task->name] = curBalances;
              size_t balanceIndex = 0;
              for(std::list<PDevice>::iterator it = _devices.begin(); it != _devices.end(); ++it) {
                (*curBalances)[balanceIndex] = (*it)->balance[task->name];
                ++balanceIndex;
              }
            } else {
              curBalances = curBalancesIterator->second;
            }
            a_pdevice->active = true;
            if (balance->duration) {
              setDuration(balance,
                          balance->duration,
                          true);
            }
            {
              Unlock unlock(a_lock);
              if (!task->split) {
                while(!task->errorFlag && (task->offset < task->size)) {
                  unsigned long long tsize   = fill(*curBalances, balance, task->packageSize);
                  unsigned long long loffset = task->offset.fetch_add(tsize);
                  if (loffset >= task->size) {
                    break;
                  }
                  tsize = std::min(tsize, task->size - loffset);

                  size_t counter = 0;
                  size_t maxCounter = 10000;
                  while(loffset + tsize > task->readyOffsetEnd) {
                    if (loffset <= task->readyOffsetEnd) {
                      Lock lock(a_lock);
                      _setStat(task);
                      task->readyOffsetEnd += task->packageSize;
                      _condition.notify_all();
                      break;
                    }
                    if (counter < maxCounter) {
                      ++counter;
                      continue;
                    } else {
                      Lock lock(a_lock);
                      if (loffset + tsize <= task->readyOffsetEnd) {
                        continue;
                      }
                      if (loffset <= task->readyOffsetEnd) {
                        continue;
                      }
                      _condition.wait(a_lock);
                    }
                  }

                  try {
                    subTask.offset      = loffset;
                    subTask.size        = tsize;
                    subTask.deviceIndex = a_pdevice->index;
                    subTask.deviceSubIndex = a_pdevice->subindex;
                    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
                    task->action(subTask, task->userData);
                    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
                    setDuration(balance,
                                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
                  } catch(std::exception& e) {
                    Lock lock(a_lock);
                    task->error     = e.what();
                    task->errorFlag = true;
                    _condition.notify_all();
                  }

                  task->completeOffset.fetch_add(tsize);
                }
              } else {
                unsigned long long npackOffset = 0;
                unsigned long long npackSize = 0;
                while(!task->errorFlag && ((task->offset < task->size) || npackOffset)) {
                  unsigned long long tsize;
                  unsigned long long loffset;
                  if (!npackSize) {
                    tsize = fill(*curBalances, balance, task->packageSize);
                    loffset = task->offset.fetch_add(tsize, std::memory_order_relaxed);
                    if (loffset >= task->size){
                      break;
                    }
                    tsize = std::min(tsize, task->size - loffset);
                    unsigned long long ccm = loffset / task->packageSize + 1;
                    unsigned long long ccp = (ccm * task->packageSize - loffset) % task->packageSize;
                    if (ccp && tsize > ccp) {
                      npackSize   = tsize - ccp;
                      npackOffset = loffset + ccp;
                      tsize       = ccp;
                    } else if (tsize > task->packageSize) {
                      npackSize   = tsize - task->packageSize;
                      npackOffset = loffset + task->packageSize;
                      tsize       = task->packageSize;
                    }
                  } else {
                    tsize = std::min(npackSize, task->packageSize);
                    loffset = npackOffset;
                    npackSize -= tsize;
                    npackOffset += tsize;
                  }
                  volatile unsigned long long &readyOffsetEnd = task->readyOffsetEnd;
                  volatile size_t counter = 0;
                  size_t maxCounter = 10000;
                  while (loffset + tsize > readyOffsetEnd) {
                    if (counter < maxCounter) {
                      ++counter;
                      continue;
                    } else {
                      Lock lock(a_lock);
                      if (loffset + tsize <= readyOffsetEnd) {
                        continue;
                      }
                      _condition.wait(a_lock);
                    }
                  }

                  try {
                    subTask.offset         = loffset;
                    subTask.size           = tsize;
                    subTask.deviceIndex    = a_pdevice->index;
                    subTask.deviceSubIndex = a_pdevice->subindex;
                    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
                    task->action(subTask, task->userData);
                    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
                    setDuration(balance,
                                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                                );
                  } catch(std::exception& e) {
                    Lock lock(a_lock);
                    task->error     = e.what();
                    task->errorFlag = true;
                    _condition.notify_all();
                  }
                  unsigned long long lastCompleteOffset = task->completeOffset.fetch_add(tsize, std::memory_order_relaxed);
                  if ((lastCompleteOffset + tsize) >= task->readyOffsetEnd) {
                    Lock lock(a_lock);
                    _setStat(task);
                    task->readyOffsetEnd += task->packageSize;
                    _condition.notify_all();
                  }
                }
              }
            }
            a_pdevice->active = false;
            _condition.notify_all();
          }

          if (_stop) {
            break;
          }


          if (_task.get() && !_task->errorFlag && _task->offset < _task->size) {
            continue;
          }

          _condition.wait(a_lock);
        }
      }
      #endif

      #ifdef FCF_PARALLEL_IMPLEMENTATION
        void Distributor::_balance(Balance& a_balance, unsigned long long a_duration){
          a_balance.duration = a_duration;
        }
      #endif


    } // Details namespace
  } // Parallel namespace
} // fcf namespace

#endif // #ifndef ___FCF__PARALLEL__DETAILS__FUNCTION_HPP___
