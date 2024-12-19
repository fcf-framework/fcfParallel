find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIR})
add_executable("parallel-test" impl.cpp
                               main.cpp
                               inner/fillTests.cpp
                               inner/DistributorTest.cpp
                               inner/FunctionDescriptorTest.cpp
                               inner/FunctionTest.cpp
                               engine/SimpleEngineTest.cpp
                               engine/splitPackageTest.cpp
                               types/charTypesTest.cpp
                               types/shortTypesTest.cpp
                               types/intTypesTest.cpp
                               types/int64TypesTest.cpp
                               types/floatTypesTest.cpp
                               types/doubleTypesTest.cpp
                              )
target_link_libraries("parallel-test" ${OpenCL_LIBRARY})

