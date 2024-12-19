#ifndef ___FCF__PARALLEL__OPENCV_ENGINE_HPP___
#define ___FCF__PARALLEL__OPENCV_ENGINE_HPP___

#include "macro.hpp"

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 120
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <regex>

#include "./Details/FunctionDescriptor.hpp"
#include "BaseEngine.hpp"
#include "Call.hpp"
#include "Task.hpp"
#include "Arg.hpp"
#include "Registrator.hpp"

#define FCF_PARALLEL_OPENCL_SYS_ARG_COUNT 4

namespace fcf {
  namespace Parallel {

    namespace Details {
      FCF_PARALLEL_DECL_EXPORT std::string getOpenCLBuildLog(cl_program a_program, cl_device_id a_deviceId);
      FCF_PARALLEL_DECL_EXPORT std::string openCLErrorToString(int a_ec);
    }

    class FCF_PARALLEL_DECL_EXPORT OpenCLEngine: public BaseEngine {
      protected:

        struct DevArg;
        typedef std::shared_ptr<DevArg> PDevArg;
        typedef std::vector<PDevArg>    DevArgs;

        struct DevCommand;
        typedef std::shared_ptr<DevCommand>         PDevCommand;
        typedef std::map<std::string, PDevCommand>  DevCommands;

        struct Device {
          cl_device_id      device;
          size_t            deviceIndex;
          cl_context        context;
          cl_command_queue  queue;
          bool              enable;
          DevCommands       commands;
          PDevCommand       currentCommand;

          Device(cl_device_id a_device, size_t a_deviceIndex, cl_context a_context, bool a_enable);
          ~Device();

          private:
            Device(Device& a_device);
        };
        typedef std::shared_ptr<Device> PDevice;
        typedef std::vector<PDevice>    Devices;


        struct DevCommand {
          public:
            cl_program compileProgram;
            cl_program libraryProgram;
            cl_program program;
            cl_kernel  kernel;
            DevArgs    args;

            DevCommand(size_t a_engineIndex, const Call& a_call, const char* a_code, PDevice a_device);
            ~DevCommand();
          private:
            DevCommand();
            DevCommand(const DevCommand&);
            void _release();
        };

        struct DevArg {
          public:
            cl_mem mem;
            size_t memSize;

            DevArg(Device* a_device, DevCommand* a_command, BaseArg* a_arg, size_t a_argIndex);
            ~DevArg();

          private:
            DevArg(const DevArg&);
            void _release();
        };

      public:
        OpenCLEngine();
        virtual ~OpenCLEngine();
        virtual void initialize(size_t a_index, Details::Distributor* a_distributor);
        virtual void prepare(const Call& a_call, Details::Distributor::Call& a_distributorCall, BaseArg** a_args, size_t a_argsc);
        virtual void applyArgs(bool a_first, const Call& a_call, BaseArg** a_args, size_t a_argsc);
        virtual void execute(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, BaseArg** a_args, size_t a_argsc);
      protected:
        std::string _prepareCode(std::string& a_code);

        bool                                      _enable;
        std::vector<PDevice>                      _devices;
        unsigned long long                        _clargsize;
        unsigned long long                        _clargoffset;
        unsigned long long                        _packageSize;
        size_t                                    _index;
    };

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::Device::Device(cl_device_id a_device, size_t a_deviceIndex, cl_context a_context, bool a_enable)
        : device(a_device)
        , deviceIndex(a_deviceIndex)
        , context(a_context)
        , queue(0)
        , enable(a_enable)
      {
        if (!enable) {
          return;
        }
        cl_int iresult = CL_SUCCESS;
        queue = clCreateCommandQueue(a_context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &iresult);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clCreateCommandQueue() failed: " + Details::openCLErrorToString(iresult));
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::Device::~Device() {
        if (queue) {
          clReleaseCommandQueue(queue);
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::DevCommand::DevCommand(size_t a_engineIndex, const Call& a_call, const char* a_code, PDevice a_device)
        : compileProgram(0)
        , libraryProgram(0)
        , program(0)
        , kernel(0)
      {
        int iresult = CL_SUCCESS;
        compileProgram = clCreateProgramWithSource(a_device->context, 1, (const char **)&a_code, 0, &iresult);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clCreateProgramWithSource failed: " + Details::openCLErrorToString(iresult));
        }
        iresult = clCompileProgram(compileProgram,
                                   1,
                                   &a_device->device,
                                   "",
                                   0,
                                   0,
                                   0,
                                   0,
                                   0
                                  );
        std::string log;
        if (iresult != CL_SUCCESS || a_call.stat){
          log = Details::getOpenCLBuildLog(compileProgram, a_device->device);
        }
        if (a_call.stat){
          if(!(*a_call.stat)["log"].is(UT_VECTOR)){
            (*a_call.stat)["log"] = Union(UT_VECTOR);
          }
          fcf::Union::iterator newItemIt = (*a_call.stat)["log"].insert(Union(UT_MAP));
          (*newItemIt)["engineIndex"] = a_engineIndex;
          (*newItemIt)["deviceIndex"] = a_device->deviceIndex;
          (*newItemIt)["action"]      = "opencl_compile";
          (*newItemIt)["message"]     = log;
        }
        if (iresult != CL_SUCCESS){
          _release();
          std::string exceptionMessage;
          exceptionMessage += "Failed execute clCompileProgram.\n";
          exceptionMessage += "\n";
          exceptionMessage += "Source code:\n";
          exceptionMessage += a_code;
          exceptionMessage += "\n";
          exceptionMessage += "\n";
          exceptionMessage += "Build log:\n";
          exceptionMessage += log;
          throw std::runtime_error(exceptionMessage);
        }

        std::mutex mutex;
        struct LinkUserData{
          cl_program* program;
          std::mutex* mutex;
        };
        mutex.lock();
        LinkUserData lud{&libraryProgram, &mutex};
        clLinkProgram(a_device->context,
                      1,
                      &a_device->device,
                      "-create-library",
                      1,
                      &compileProgram,
                      [](cl_program a_program, void* a_ud){
                        LinkUserData* lud = (LinkUserData*)a_ud;
                        *lud->program = a_program;
                        lud->mutex->unlock();
                      },
                      &lud,
                      &iresult
                     );
        mutex.lock();
        mutex.unlock();
        cl_build_status status = CL_BUILD_ERROR;
        clGetProgramBuildInfo(libraryProgram, a_device->device, CL_PROGRAM_BUILD_STATUS, sizeof(status), &status, 0);
        if (status != CL_BUILD_SUCCESS && iresult == CL_SUCCESS) {
          iresult = CL_LINK_PROGRAM_FAILURE;
        }
        if (iresult != CL_SUCCESS || a_call.stat) {
          log = Details::getOpenCLBuildLog(libraryProgram, a_device->device);
        }
        if (a_call.stat) {
          fcf::Union::iterator newItemIt = (*a_call.stat)["log"].insert(Union(UT_MAP));
          (*newItemIt)["engineIndex"] = a_engineIndex;
          (*newItemIt)["deviceIndex"] = a_device->deviceIndex;
          (*newItemIt)["action"]      = "opencl_link_modules";
          (*newItemIt)["message"]     = log;
        }
        if (iresult != CL_SUCCESS){
          _release();
          std::string exceptionMessage;
          exceptionMessage += "Invalid linking module. Failed execute clLinkProgram.\n";
          exceptionMessage += "\n";
          exceptionMessage += "Source code:\n";
          exceptionMessage += a_code;
          exceptionMessage += "\n";
          exceptionMessage += "\n";
          exceptionMessage += "Link log:\n";
          exceptionMessage += log;
          throw std::runtime_error(exceptionMessage);
        }
        mutex.lock();
        lud = LinkUserData{&program, &mutex};
        clLinkProgram(a_device->context,
                      1,
                      &a_device->device,
                      "",
                      1,
                      &libraryProgram,
                      [](cl_program a_program, void* a_ud){
                        LinkUserData* lud = (LinkUserData*)a_ud;
                        *lud->program = a_program;
                        lud->mutex->unlock();
                      },
                      &lud,
                      &iresult
                     );
        mutex.lock();
        status = CL_BUILD_ERROR;
        clGetProgramBuildInfo(program, a_device->device, CL_PROGRAM_BUILD_STATUS, sizeof(status), &status, 0);
        if (status != CL_BUILD_SUCCESS && iresult == CL_SUCCESS) {
          iresult = CL_LINK_PROGRAM_FAILURE;
        }

        if (iresult != CL_SUCCESS || a_call.stat) {
          log = Details::getOpenCLBuildLog(program, a_device->device);
        }
        if (a_call.stat) {
          fcf::Union::iterator newItemIt = (*a_call.stat)["log"].insert(Union(UT_MAP));
          (*newItemIt)["engineIndex"] = a_engineIndex;
          (*newItemIt)["deviceIndex"] = a_device->deviceIndex;
          (*newItemIt)["action"]      = "opencl_link_program";
          (*newItemIt)["message"]     = log;
        }
        if (iresult != CL_SUCCESS){
          _release();
          std::string exceptionMessage;
          exceptionMessage += "Invalid linking program. Failed execute clLinkProgram.\n";
          exceptionMessage += "\n";
          exceptionMessage += "Source code:\n";
          exceptionMessage += a_code;
          exceptionMessage += "\n";
          exceptionMessage += "\n";
          exceptionMessage += "Link log:\n";
          exceptionMessage += log;
          throw std::runtime_error(exceptionMessage);
        }
        kernel = clCreateKernel(program, "__FCF_PARALLEL_MAIN", &iresult);
        if (iresult) {
          _release();
          throw std::runtime_error(std::string() + "clCreateKernel failed: " + Details::openCLErrorToString(iresult));
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::DevCommand::~DevCommand() {
        _release();
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void OpenCLEngine::DevCommand::_release() {
        if (kernel) {
          clReleaseKernel(kernel);
          kernel = 0;
        }
        if (compileProgram) {
          clReleaseProgram(compileProgram);
          compileProgram = 0;
        }
        if (libraryProgram) {
          clReleaseProgram(libraryProgram);
          libraryProgram = 0;
        }
        if (program) {
          clReleaseProgram(program);
          program = 0;
        }
      }
   #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::DevArg::DevArg(Device* a_device, DevCommand* a_command, BaseArg* a_arg, size_t a_argIndex)
        : mem(0)
        , memSize(0)
      {
        cl_int iresult = CL_SUCCESS;
        if (!a_arg->container) {
          iresult = clSetKernelArg(a_command->kernel,
                                   a_argIndex + FCF_PARALLEL_OPENCL_SYS_ARG_COUNT,
                                   a_arg->size,
                                   a_arg->pointer);
          if (iresult != CL_SUCCESS) {
            throw std::runtime_error(std::string() + "clSetKernelArg(" + std::to_string(a_argIndex) + ") failed: " + Details::openCLErrorToString(iresult));
          }
        } else {
          size_t dsize = a_arg->itemSize * a_arg->length;
          mem = clCreateBuffer(a_device->context,
                                (a_arg->upload ? (CL_MEM_READ_WRITE) : CL_MEM_READ_ONLY),
                               dsize,
                               0,
                               &iresult);
          if (!mem) {
            throw std::runtime_error(std::string() + "clCreateBuffer(" + std::to_string(a_argIndex) + ") failed (for container): " + Details::openCLErrorToString(iresult));
          }
          memSize = dsize;
          iresult = clEnqueueWriteBuffer(a_device->queue,
                                         mem,
                                         CL_TRUE,
                                         0,
                                         dsize,
                                         *(void**)a_arg->pointer,
                                         0,
                                         NULL,
                                         NULL);
          if (iresult != CL_SUCCESS) {
            throw std::runtime_error(std::string() + "clEnqueueWriteBuffer (argNumber:" + std::to_string(a_argIndex) + ") failed: " + Details::openCLErrorToString(iresult));
          }
          iresult = clSetKernelArg(a_command->kernel,
                                   a_argIndex + FCF_PARALLEL_OPENCL_SYS_ARG_COUNT,
                                   sizeof(cl_mem),
                                   &mem);
          if (iresult != CL_SUCCESS) {
            throw std::runtime_error(std::string() + "clSetKernelArg(" + std::to_string(a_argIndex) + ") failed: " + Details::openCLErrorToString(iresult));
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::DevArg::~DevArg(){
        _release();
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void OpenCLEngine::DevArg::_release(){
        if (mem) {
          clReleaseMemObject(mem);
          mem = 0;
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::OpenCLEngine() {
        _enable = false;
        _index = 0;
        _properties["name"]        = "opencl";
        _properties["devices"]     = fcf::Union(fcf::UT_VECTOR);
        _properties["threads"]     = 0;
        _properties["minDuration"] = 40 * 1000 * 1000;

        cl_uint platformIdResC = 0;
        cl_platform_id platformIds[128];
        cl_uint result = 0;
        result = clGetPlatformIDs(sizeof(platformIds) / sizeof(platformIds[0]), &platformIds[0], &platformIdResC);
        if (result != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clGetPlatformIDs failed: " + Details::openCLErrorToString(result));
        }
        for(size_t platformIndex = 0; platformIndex < platformIdResC; ++platformIndex) {
          cl_device_id devicev[64];
          cl_uint devicevc = sizeof(devicev) / sizeof(devicev[0]);
          cl_uint devicevdc = 0;
          result = clGetDeviceIDs(platformIds[platformIndex], CL_DEVICE_TYPE_ALL, devicevc, &devicev[0], &devicevdc);
          if (result != CL_SUCCESS) {
            continue;
          }
          for(cl_uint deviceIndex = 0; deviceIndex < devicevdc; ++deviceIndex) {
            size_t         paramSizeRes = 0;

            cl_device_type deviceType;
            result = clGetDeviceInfo(
                      devicev[deviceIndex],
                      CL_DEVICE_TYPE,
                      sizeof(deviceType),
                      &deviceType,
                      &paramSizeRes
                    );
            if (result != CL_SUCCESS) {
              throw std::runtime_error(std::string() + "clGetDeviceInfo(CL_DEVICE_TYPE) failed: " + Details::openCLErrorToString(result));
            }

            std::string deviceName;
            size_t      deviceNameSize = 0;
            result = clGetDeviceInfo(
                      devicev[deviceIndex],
                      CL_DEVICE_NAME,
                      0,
                      0,
                      &deviceNameSize
                    );
            if (result != CL_SUCCESS) {
              throw std::runtime_error(std::string() + "clGetDeviceInfo(CL_DEVICE_TYPE) failed: " + Details::openCLErrorToString(result));
            }
            deviceName.resize(std::max(deviceNameSize, (size_t)1) - 1);
            result = clGetDeviceInfo(
                      devicev[deviceIndex],
                      CL_DEVICE_NAME,
                      deviceName.length() + 1,
                      &deviceName[0],
                      &deviceNameSize
                    );
            if (result != CL_SUCCESS) {
              throw std::runtime_error(std::string() + "clGetDeviceInfo(CL_DEVICE_TYPE) failed: " + Details::openCLErrorToString(result));
            }

            cl_uint deviceComputeUnits = 0;
            result = clGetDeviceInfo(
                      devicev[deviceIndex],
                      CL_DEVICE_MAX_COMPUTE_UNITS,
                      sizeof(deviceComputeUnits),
                      &deviceComputeUnits,
                      &paramSizeRes
                    );
            if (result != CL_SUCCESS) {
              throw std::runtime_error(std::string() + "clGetDeviceInfo(CL_DEVICE_MAX_COMPUTE_UNITS) failed: " + Details::openCLErrorToString(result));
            }


            size_t wgSize = 0;
            result = clGetDeviceInfo(
                      devicev[deviceIndex],
                      CL_DEVICE_MAX_WORK_GROUP_SIZE,
                      sizeof(wgSize),
                      &wgSize,
                      &paramSizeRes
                    );
            if (result != CL_SUCCESS) {
              throw std::runtime_error(std::string() + "clGetDeviceInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed: " + Details::openCLErrorToString(result));
            }

           _properties["threads"].ref<unsigned int>() += deviceComputeUnits * wgSize * 0.2;

            fcf::Union udev(fcf::UT_MAP);
            udev["enable"]  = deviceType != CL_DEVICE_TYPE_CPU;
            udev["name"]    = deviceName;
            udev["id"]      = (unsigned long long)devicev[deviceIndex];
            udev["threads"] = (unsigned long long)(deviceComputeUnits * wgSize * 0.2);
            udev["type"]    = deviceType == CL_DEVICE_TYPE_CPU         ? "cpu" :
                              deviceType == CL_DEVICE_TYPE_GPU         ? "gpu" :
                              deviceType == CL_DEVICE_TYPE_ACCELERATOR ? "accelerator" :
                                                                         "custom";
            _properties["devices"].insert(udev);
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      OpenCLEngine::~OpenCLEngine(){
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void OpenCLEngine::initialize(size_t a_index, Details::Distributor* a_distributor) {
        _index = a_index;
        if (!(bool)_properties["enable"]) {
          return;
        }
        fcf::Union& udevs = _properties["devices"];
        std::vector<cl_device_id> devices;
        for(fcf::Union& udev : udevs){
          if (!(unsigned long long)udev["threads"]){
            udev["enable"] = false;
          }
          if ((bool)udev["enable"]){
            cl_device_id did = (cl_device_id)(unsigned long long)udev["id"];
            devices.push_back(did);
          }
        }

        cl_int iresult = CL_SUCCESS;
        for(fcf::Union& udev : udevs){
          cl_device_id deviceId = (cl_device_id)(unsigned long long)udev["id"];
          cl_context context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &iresult);
          if (iresult || !context) {
            throw std::runtime_error(std::string() + "clCreateContext failed: " + Details::openCLErrorToString(iresult));
          }
          PDevice pdevice(new Device(deviceId, _devices.size(), context, (bool)udev["enable"]));
          _devices.push_back(pdevice);
          if (pdevice->enable) {
            unsigned long long minDuration = udev["minDuration"].isCompatible<unsigned long long>()
                                              ? (unsigned long long)udev["minDuration"]
                                              : (unsigned long long)_properties["minDuration"];
            a_distributor->addDevice(a_index, pdevice->deviceIndex, (unsigned long long)udev["threads"], minDuration);
          }
        }

        _enable = !!devices.size();
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void OpenCLEngine::prepare(const Call& a_call, Details::Distributor::Call& a_distributorCall, BaseArg** a_args, size_t a_argsc) {
        if (!_enable){
          return;
        }
        _packageSize = a_call.packageSize;
        BaseEngine::prepare(a_call, a_distributorCall, a_args, a_argsc);

        std::string code;
        for(PDevice& pdevice : _devices){
          if (pdevice->enable) {
            DevCommands::iterator itCommand = pdevice->commands.find(a_call.name);
            if (itCommand == pdevice->commands.end()){
              if (code.empty()){
                PUnit punit = Registrator().get(a_call.name);
                code = _prepareCode(punit->code);
              }
              PDevCommand command;
              try {
                command.reset(new DevCommand(_index, a_call, code.c_str(), pdevice));
              } catch(const std::exception& e) {
                a_distributorCall.lastError = e.what();
              }
              itCommand = pdevice->commands.insert(DevCommands::value_type(a_call.name, command)).first;
            }
            if (!itCommand->second){
              a_distributorCall.ignoreDevice.push_back(Details::Distributor::DeviceIndex{_index, pdevice->deviceIndex});
            }
            pdevice->currentCommand = itCommand->second;
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void OpenCLEngine::applyArgs(bool a_first, const Call& a_call, BaseArg** a_args, size_t a_argsc){
        if (a_first) {
          for(PDevice& pdevice : _devices){
            if (pdevice->enable && pdevice->currentCommand) {
              pdevice->currentCommand->args.resize(a_argsc);
              for(size_t argIndex = 0; argIndex < a_argsc; ++argIndex) {
                PDevArg arg(new DevArg(pdevice.get(), pdevice->currentCommand.get(), a_args[argIndex], argIndex));
                pdevice->currentCommand->args[argIndex] = arg;
              }
            }
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      void OpenCLEngine::execute(const fcf::Parallel::Details::Distributor::SubTask& a_subtask, BaseArg** a_args, size_t a_argsc) {
        cl_int  iresult = 0;
        _clargoffset = a_subtask.offset;
        _clargsize = a_subtask.size;
        PDevice&     device  = _devices[a_subtask.deviceSubIndex];
        PDevCommand& command = device->currentCommand;
        if (!command){
          throw std::runtime_error("Invalid logic");
        }

        unsigned int lowIndex  = a_subtask.offset & 0xffffffff;
        unsigned int highIndex = a_subtask.offset >> 32;
        unsigned int packageIndex = (unsigned int)(a_subtask.offset % _packageSize);
        unsigned int taskSize  = a_subtask.size;

        iresult = clSetKernelArg(command->kernel, 0, sizeof(lowIndex), &lowIndex);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clSetKernelArg(SYS:0) failed: " + Details::openCLErrorToString(iresult));
        }
        iresult = clSetKernelArg(command->kernel, 1, sizeof(highIndex), &highIndex);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clSetKernelArg(SYS:1) failed: " + Details::openCLErrorToString(iresult));
        }
        iresult = clSetKernelArg(command->kernel, 2, sizeof(packageIndex), &packageIndex);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clSetKernelArg(SYS:2) failed: " + Details::openCLErrorToString(iresult));
        }
        iresult = clSetKernelArg(command->kernel, 3, sizeof(taskSize), &taskSize);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clSetKernelArg(SYS:3) failed: " + Details::openCLErrorToString(iresult));
        }
        size_t localSize = 0;
        size_t globalSize = 0;
        cl_device_id deviceId = (cl_device_id)(unsigned long long)_properties["devices"][a_subtask.deviceSubIndex]["id"];
        iresult = clGetKernelWorkGroupInfo(command->kernel,
                                           deviceId,
                                           CL_KERNEL_WORK_GROUP_SIZE,
                                           sizeof(localSize),
                                           &localSize, NULL);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error("clGetKernelWorkGroupInfo failed. Code " + Details::openCLErrorToString(iresult));
        }
        globalSize = std::ceil(a_subtask.size/(double)localSize) * localSize;

        iresult = clEnqueueNDRangeKernel(
                    device->queue,
                    command->kernel,
                    1,
                    0,
                    &globalSize,
                    &localSize,
                    0,
                    NULL,
                    NULL);
        if (iresult != CL_SUCCESS) {
          throw std::runtime_error(std::string() + "clEnqueueNDRangeKernel failed: " + Details::openCLErrorToString(iresult));
        }
        clFinish(device->queue);

        for(size_t argIndex = 0; argIndex < a_argsc; ++argIndex) {
          if (command->args[argIndex]->mem && a_args[argIndex]->upload) {
            size_t itemSize = a_args[argIndex]->itemSize * a_args[argIndex]->splitSize;
            unsigned long long offset = a_subtask.offset;
            if (a_args[argIndex]->split == PS_PACKAGE) {
              offset = offset % _packageSize;
            }
            iresult = clEnqueueReadBuffer(device->queue,
                                          command->args[argIndex]->mem,
                                          CL_TRUE,
                                          itemSize * offset,
                                          itemSize * a_subtask.size,
                                          (*(char**)a_args[argIndex]->pointer) + itemSize * offset,
                                          0,
                                          NULL,
                                          NULL);
            if (iresult != CL_SUCCESS) {
              throw std::runtime_error(std::string() + "clEnqueueReadBuffer failed: " + Details::openCLErrorToString(iresult));
            }
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      std::string OpenCLEngine::_prepareCode(std::string& a_code) {
        std::string code = std::regex_replace(a_code, std::regex("FCF_PARALLEL_GLOBAL"), "__global");

        Details::FunctionDescriptor fdf(code, "FCF_PARALLEL_MAIN", false);
        Details::FunctionDescriptor fd(code, "FCF_PARALLEL_MAIN", true);

        std::string prefix = std::string() +
          "struct _FCFParallelTask\n"
          "{\n"
          "  unsigned int lowIndex;\n"
          "  unsigned int highIndex;\n"
          "  unsigned int packageIndex;\n"
          "};\n"
          "typedef struct _FCFParallelTask FCFParallelTask;\n"
          "typedef char fcf_int8;\n"
          "typedef unsigned char fcf_uint8;\n"
          "typedef short fcf_int16;\n"
          "typedef unsigned short fcf_uint16;\n"
          "typedef int fcf_int32;\n"
          "typedef unsigned int fcf_uint32;\n"
          "typedef long fcf_int64;\n"
          "typedef unsigned long fcf_uint64;\n"
          "typedef float fcf_float32;\n"
          "typedef double fcf_float64;\n"
          "inline void FCF_PARALLEL_MAIN(" + fdf.declArgs + ");\n"
          "\n"
          "__kernel void __FCF_PARALLEL_MAIN(unsigned int a_fcf_parallel_low_offset,\n"
          "                                  unsigned int a_fcf_parallel_high_offset,\n"
          "                                  unsigned int a_fcf_parallel_package_offset,\n"
          "                                  unsigned int a_fcf_parallel_task_size\n"
          "                                  " + (!fd.declArgs.empty() ? ", " : "") + fd.declArgs + ") {\n"
          "  unsigned int var_fcf_parallel_task_index = (unsigned int)get_global_id(0);\n"
          "  FCFParallelTask var_fcf_parallel_task;\n"
          "  var_fcf_parallel_task.lowIndex  = var_fcf_parallel_task_index + a_fcf_parallel_low_offset;\n"
          "  var_fcf_parallel_task.highIndex = a_fcf_parallel_high_offset;\n"
          "  var_fcf_parallel_task.packageIndex = a_fcf_parallel_package_offset + var_fcf_parallel_task_index;\n"
          "  if (var_fcf_parallel_task.lowIndex < var_fcf_parallel_task_index) {\n"
          "    ++var_fcf_parallel_task.highIndex;\n"
          "  }\n"
          "  if (var_fcf_parallel_task_index > a_fcf_parallel_task_size) {\n"
          "    return; \n"
          "  }\n"
          "  FCF_PARALLEL_MAIN(&var_fcf_parallel_task" + (!fd.args.empty() ? ", " : "") + fd.args + ");\n"
          "}\n"
          ;
        return prefix + code;
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      namespace Details { 
        std::string getOpenCLBuildLog(cl_program a_program, cl_device_id a_deviceId) {
          size_t logSize = 0;
          cl_int iresult = clGetProgramBuildInfo(a_program, a_deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
          if (iresult) {
            return std::string("[Failed to get a build log]");
          }
          std::string log;
          log.resize(logSize+1);

          iresult = clGetProgramBuildInfo(a_program, a_deviceId, CL_PROGRAM_BUILD_LOG, logSize, &log[0], NULL);
          if (iresult) {
            return std::string("[Failed to get a build log]");
          }

          return log;
        }
      } // Details namespace
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

    #ifdef FCF_PARALLEL_IMPLEMENTATION
      namespace Details { 
        std::string openCLErrorToString(int a_ec) {
          switch (a_ec) {
            case 0: return "CL_SUCCESS";
            case -1: return "CL_DEVICE_NOT_FOUND";
            case -2: return "CL_DEVICE_NOT_AVAILABLE";
            case -3: return "CL_COMPILER_NOT_AVAILABLE";
            case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
            case -5: return "CL_OUT_OF_RESOURCES";
            case -6: return "CL_OUT_OF_HOST_MEMORY";
            case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
            case -8: return "CL_MEM_COPY_OVERLAP";
            case -9: return "CL_IMAGE_FORMAT_MISMATCH";
            case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
            case -12: return "CL_MAP_FAILURE";
            case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
            case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
            case -15: return "CL_COMPILE_PROGRAM_FAILURE";
            case -16: return "CL_LINKER_NOT_AVAILABLE";
            case -17: return "CL_LINK_PROGRAM_FAILURE";
            case -18: return "CL_DEVICE_PARTITION_FAILED";
            case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
            case -30: return "CL_INVALID_VALUE";
            case -31: return "CL_INVALID_DEVICE_TYPE";
            case -32: return "CL_INVALID_PLATFORM";
            case -33: return "CL_INVALID_DEVICE";
            case -34: return "CL_INVALID_CONTEXT";
            case -35: return "CL_INVALID_QUEUE_PROPERTIES";
            case -36: return "CL_INVALID_COMMAND_QUEUE";
            case -37: return "CL_INVALID_HOST_PTR";
            case -38: return "CL_INVALID_MEM_OBJECT";
            case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
            case -40: return "CL_INVALID_IMAGE_SIZE";
            case -41: return "CL_INVALID_SAMPLER";
            case -42: return "CL_INVALID_BINARY";
            case -43: return "CL_INVALID_BUILD_OPTIONS";
            case -44: return "CL_INVALID_PROGRAM";
            case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
            case -46: return "CL_INVALID_KERNEL_NAME";
            case -47: return "CL_INVALID_KERNEL_DEFINITION";
            case -48: return "CL_INVALID_KERNEL";
            case -49: return "CL_INVALID_ARG_INDEX";
            case -50: return "CL_INVALID_ARG_VALUE";
            case -51: return "CL_INVALID_ARG_SIZE";
            case -52: return "CL_INVALID_KERNEL_ARGS";
            case -53: return "CL_INVALID_WORK_DIMENSION";
            case -54: return "CL_INVALID_WORK_GROUP_SIZE";
            case -55: return "CL_INVALID_WORK_ITEM_SIZE";
            case -56: return "CL_INVALID_GLOBAL_OFFSET";
            case -57: return "CL_INVALID_EVENT_WAIT_LIST";
            case -58: return "CL_INVALID_EVENT";
            case -59: return "CL_INVALID_OPERATION";
            case -60: return "CL_INVALID_GL_OBJECT";
            case -61: return "CL_INVALID_BUFFER_SIZE";
            case -62: return "CL_INVALID_MIP_LEVEL";
            case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
            case -64: return "CL_INVALID_PROPERTY";
            case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
            case -66: return "CL_INVALID_COMPILER_OPTIONS";
            case -67: return "CL_INVALID_LINKER_OPTIONS";
            case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
            case -69: return "CL_INVALID_PIPE_SIZE";
            case -70: return "CL_INVALID_DEVICE_QUEUE";
            case -71: return "CL_INVALID_SPEC_ID";
            case -72: return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
            case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
            case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
            case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
            case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
            case -1006: return "CL_INVALID_D3D11_DEVICE_KHR";
            case -1007: return "CL_INVALID_D3D11_RESOURCE_KHR";
            case -1008: return "CL_D3D11_RESOURCE_ALREADY_ACQUIRED_KHR";
            case -1009: return "CL_D3D11_RESOURCE_NOT_ACQUIRED_KHR";
            case -1010: return "CL_INVALID_DX9_MEDIA_ADAPTER_KHR";
            case -1011: return "CL_INVALID_DX9_MEDIA_SURFACE_KHR";
            case -1012: return "CL_DX9_MEDIA_SURFACE_ALREADY_ACQUIRED_KHR";
            case -1013: return "CL_DX9_MEDIA_SURFACE_NOT_ACQUIRED_KHR";
            case -1093: return "CL_INVALID_EGL_OBJECT_KHR";
            case -1092: return "CL_EGL_RESOURCE_NOT_ACQUIRED_KHR";
            case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
            case -1057: return "CL_DEVICE_PARTITION_FAILED_EXT";
            case -1058: return "CL_INVALID_PARTITION_COUNT_EXT";
            case -1059: return "CL_INVALID_PARTITION_NAME_EXT";
            case -1094: return "CL_INVALID_ACCELERATOR_INTEL";
            case -1095: return "CL_INVALID_ACCELERATOR_TYPE_INTEL";
            case -1096: return "CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL";
            case -1097: return "CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL";
            case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
            case -1098: return "CL_INVALID_VA_API_MEDIA_ADAPTER_INTEL";
            case -1099: return "CL_INVALID_VA_API_MEDIA_SURFACE_INTEL";
            case -1100: return "CL_VA_API_MEDIA_SURFACE_ALREADY_ACQUIRED_INTEL";
            case -1101: return "CL_VA_API_MEDIA_SURFACE_NOT_ACQUIRED_INTEL";
            default: return std::string("CL_UNKNOWN_ERROR:") + std::to_string(a_ec);
          }
        }
      }
    #endif // #ifdef FCF_PARALLEL_IMPLEMENTATION

  } // Parallel namespace
} // fcf namespace
#endif // #ifndef ___FCF__PARALLEL__OPENCV_ENGINE_HPP___


