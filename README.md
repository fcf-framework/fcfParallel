## C++11 fcfParallel library for parallel calculations 

<a name="short_description"></a>
### Brief description

C++11 header library for CPU/GPU parallel computing uses native compiler and OpenCL.

### Attention! Cloning the library with the option --recursive

**Example:**

```bash
git clone --recursive https://github.com/fcf-framework/fcfParallel.git
```

### Fast start. Simple example

The simple example of overlaying blur in a BMP file. The example is available in the repository https://github.com/fcf-framework/fcfParallelExamples.git

**impl.cpp file**

Since the library is implemented in the form of header files, first you need to announce its implementation. 
To do this, you need to connect the header file `fcfParallel/parallel.hpp` with the declared macro `FCF_PARALLEL_IMPLEMENTATION` (only for fcfParallel library) or `FCF_IMPLEMENTATION` (for all libraries).
It is advisable to do this in a separate file so as not to reassemble each time.

```c++
#define FCF_IMPLEMENTATION
#include <fcfImage/image.hpp>
#include <fcfParallel/parallel.hpp>
```

**main.cpp file**

We now proceed to the main programm.

*Declaring a handler function*

The parallel computation subroutine is declared the macro FCF_PARALLEL_UNIT. The first parameter is the name of the unit, and the second will be the code for the implementation of the action.

The subroutine should contain a main function called FCF_PARALLEL_MAIN. The first argument of this function should be a pointer to the `FCFParallelTask` structure, which contains progress information. The rest of the argument is set by the developer.

When transferring arguments by pointer from the main program to `FCF_PARALLEL_MAIN`, it is necessary to use the macro `FCF_PARALLEL_GLOBAL` when declaring them, which is analogous to the specificator `__global__` of the OpenCL compiler.

*Launching parallel computing*

To run parallel calculations, the `fcf:::Parallel::Executor` object is used.

To perform the action, call the operator() method.

The first argument is a reference to the object `fcf:::Parallel::Call`, which contains action parameters. The remaining arguments correspond to the 1-N arguments of the `FCF_PARALLEL_MAIN` function.

Transfer of arguments to operator() method:

1. The transfer of the argument by value is done simply by transferring the value.

2. If the argument of the function `FCF_PARALLEL_MAIN` is a pointer and does not require a calculation result, then you need to use the function `fcf:::Parallel:::refArg` to transfer data. The source data can be either an index or an object `std::vector`.

3. If the argument is a pointer in the memory of which the result of the calculation is recorded, then the function `fcf::::Parallel:::refArg` should contain the following parameters when transmitting the argument:

	```
	fcf::Parallel::refArg(
	  outputRGB,

	  // Indicates that after performing calculations,
	  // you need to unload the result
	  fcf::Parallel::ArgUpload(true),

	  // When enabling upload, you must specify
	  // the split parameter.
	  // This parameter indicates that the data during unloading is divided
	  // between devices, and their size corresponds to the number of iterations.
	  fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL),

	  // Number of elements per iteration
	  fcf::Parallel::ArgSplitSize(3)
    )
	```

The example of the program itself is below.

```c++
#include <iostream>
#include <fcfImage/image.hpp>
#include <fcfParallel/parallel.hpp>

//
// Pixel processing function performed on CPU/GPU
//
FCF_PARALLEL_UNIT(
    blur,
    void FCF_PARALLEL_MAIN(const FCFParallelTask* a_task,
                           int a_blur,
                           int a_width,
                           int a_height,
                           FCF_PARALLEL_GLOBAL const char* a_source,
                           FCF_PARALLEL_GLOBAL char* a_result) {
      int offset = a_task->lowIndex * 3;
      int y = a_task->lowIndex / a_width;
      int x = a_task->lowIndex % a_width;
      int begby = max(y - a_blur, 0);
      int endby = min(y + a_blur + 1, a_height);
      int begbx = max(x - a_blur, 0);
      int endbx = min(x + a_blur + 1, a_width);
      int c     = (endby - begby) * (endbx - begbx);

      for(int channel = 0; channel < 3; ++channel) {
        int value = 0;
        for(int by = begby; by < endby; ++by) {
          for(int bx = begbx; bx < endbx; ++bx) {
            int bRawIndex = (by * a_width + bx) * 3;
            value += (int)(unsigned char)a_source[bRawIndex + channel];
          }
        }
        a_result[offset + channel] = (char)(value / c);
      }
    }
)

void printHelp(){
  std::cout << "An example application illustrating the use of fcfParallel" << std::endl;
  std::cout << "  Application launch format: parallel-example-001-blur SOURCE_BMP_FILE OUTPUT_BMP_FILE" << std::endl;
  std::cout << "  Options:" << std::endl;
  std::cout << "    SOURCE_BMP_FILE - Source BMP file." << std::endl;
  std::cout << "    OUTPUT_BMP_FILE - Resulting BMP file with the blur effect applied." << std::endl;
}

int main(int a_argc, char* a_argv[]){
  //
  // Processing command line arguments
  std::string sourceFilePath;
  std::string outputFilePath;
  for(int i = 1; i < a_argc; ++i) {
    if (!std::strcmp(a_argv[i], "-h") || !std::strcmp(a_argv[i], "--help")) {
      printHelp();
      return 0;
    } else if (sourceFilePath.empty()) {
      sourceFilePath = a_argv[i];
    } else if (outputFilePath.empty()) {
      outputFilePath = a_argv[i];
    }

  }
  if (sourceFilePath.empty() || outputFilePath.empty()) {
    std::cout << "Incorrent command line parameters. Use --help option for got help." << std::endl;
    return 1;
  }

  //
  // Loading BMP image from file
  std::vector<char> sourceRGB;
  size_t            sourceRGBWidth;
  size_t            sourceRGBHeight;
  try {
    fcf::Image::loadRGBFromBmpFile(sourceFilePath, sourceRGB, sourceRGBWidth, sourceRGBHeight);
  } catch(std::exception& e){
    std::cerr << "Invalid load BMP file: " << e.what() << std::endl;
    return 1;
  }

  // Result image
  std::vector<char> outputRGB(sourceRGB.size());

  //The object that will contain debugging information
  fcf::Union state;

  //
  // Performing parallel calculations
  try {
    // Object initialization
    fcf::Parallel::Executor executor;
    executor.initialize();

    fcf::Parallel::Call call;
    // Unit name to execute declared macro FCF_PARALLEL_UNIT
    call.name = "blur";
    // Number of iterations
    call.size = sourceRGBWidth * sourceRGBHeight;
    // Specify the object in which you will need to record debugging information
    call.state = &state;

    // Running parallel computing
    executor(call,
             (unsigned int)5,
             (unsigned int)sourceRGBWidth,
             (unsigned int)sourceRGBHeight,
             fcf::Parallel::refArg(sourceRGB),
             fcf::Parallel::refArg(outputRGB,
                                   fcf::Parallel::ArgSplit(fcf::Parallel::PS_FULL),
                                   fcf::Parallel::ArgUpload(true),
                                   fcf::Parallel::ArgSplitSize(3)
                                  )
             );
  } catch(std::exception& e) {
    std::cerr << "Error in performing parallel calculations: " << e.what() << std::endl;
    return 1;
  }

  //
  // Record the result in a BMP file
  try {
    fcf::Image::writeRGBToBmpFile(outputFilePath, outputRGB, sourceRGBWidth, sourceRGBHeight);
  } catch(std::exception& e){
    std::cerr << "Invalid write BMP file: " << e.what() << std::endl;
    return 1;
  }

  //
  // Displaying debugging information
  std::cout << "Time spent on implementation: " << ((double)state["duration"]/(1000*1000*1000)) << " sec" << std::endl;
  std::cout << "Actions performed on the following devices: " << std::endl;
  for(fcf::Union& dev : state["devices"]) {
    std::cout << "    Engine: "<< dev["engine"] << "; Device: " << dev["device"] << std::endl;
  }

  return 0;
}
```

**CMakeLists.txt file (Build)**

The build parameters are presented on the basis of CMake.

In order to collect this example, you will need to link and include OpenCL. (https://github.com/KhronosGroup/OpenCL-SDK).

 ```cmake
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libraries)

add_executable("blur" impl.cpp main.cpp)
target_link_libraries("blur" ${OpenCL_LIBRARY})
 ```