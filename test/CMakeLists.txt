find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIR})
add_executable("parallel-test" impl.cpp
                               main.cpp
                               tests/DistributorTest.cpp
                               tests/types/charTypesTest.cpp
                               tests/types/shortTypesTest.cpp
                               tests/types/intTypesTest.cpp
                               tests/types/int64TypesTest.cpp
                               tests/types/floatTypesTest.cpp
                               tests/types/doubleTypesTest.cpp
                              )
target_link_libraries("parallel-test" ${OpenCL_LIBRARY})

