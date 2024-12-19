cmake_minimum_required(VERSION 3.0)

include_directories(${OpenCL_INCLUDE_DIR})

add_library("shared-library-lib" SHARED ./shared.cpp)
target_compile_definitions("shared-library-lib" PRIVATE FCF_UNION_EXPORT)
target_compile_definitions("shared-library-lib" PRIVATE FCF_PARALLEL_EXPORT)
target_link_libraries("shared-library-lib" ${OpenCL_LIBRARY})

add_executable("shared-library-app" app-main.cpp)
target_compile_definitions("shared-library-app" PRIVATE FCF_UNION_IMPORT)
target_compile_definitions("shared-library-app" PRIVATE FCF_PARALLEL_IMPORT)
target_link_libraries("shared-library-app" PRIVATE "shared-library-lib")

