cmake_minimum_required(VERSION 3.1)
project(WebXplorer64 CXX)

set(WAVM_ENABLE_STATIC_LINKING ON)
set(WAVM_ENABLE_FUZZ_TARGETS OFF)
add_subdirectory(WAVM)

set(CMAKE_CXX_STANDARD 17)

include_directories($ENV{IDASDK}/include)
add_library(WebXplorer64 SHARED WebXplorer.cpp WASMLinker.h WebXplorer.h)
target_link_libraries(WebXplorer64 $ENV{IDASDK}/lib/x64_linux_gcc_64/libida64.so libWAVM)
target_compile_definitions(WebXplorer64 PRIVATE __LINUX__)
target_compile_definitions(WebXplorer64 PRIVATE __EA64__=1)
target_compile_definitions(WebXplorer64 PRIVATE __IDP__ MAXSTR=1024)
set_target_properties(WebXplorer64 PROPERTIES LIBRARY_OUTPUT_DIRECTORY $ENV{IDABIN}/plugins)
set_target_properties(WebXplorer64 PROPERTIES PREFIX "")