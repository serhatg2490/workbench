cmake_minimum_required(VERSION 3.21)
project(workbench LANGUAGES CXX)

# Header-only library: an INTERFACE target carries include path + C++ standard
add_library(workbench INTERFACE)
add_library(workbench::workbench ALIAS workbench)
target_include_directories(workbench INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(workbench INTERFACE cxx_std_20)

option(WORKBENCH_BUILD_TESTS "Build workbench tests" ${PROJECT_IS_TOP_LEVEL})
if(WORKBENCH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
