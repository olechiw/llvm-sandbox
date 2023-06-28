cmake_minimum_required(VERSION 3.10.0)
project(TestProject)

# Hacking this in here for my local installation

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Now build our tools

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

set(CMAKE_CXX_STANDARD 17) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(simple-tool main.cpp CPPInterpreter.cpp CPPInterpreter.h)
# Find the libraries that correspond to the LLVM components
# that we wish to use
# linking shared rn
set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Analysis
  CodeGen
  Core
  IPO
  AggressiveInstCombine
  InstCombine
  Instrumentation
  MC
  MCParser
  ObjCARCOpts
  Option
  ScalarOpts
  Support
  TargetParser
  TransformUtils
  Vectorize
  )
llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})
#message(STATUS ${llvm_libs})
# Link against LLVM libraries
target_link_libraries(simple-tool clangBasic clangDriver clangFrontend clangTooling clangFrontendTool  clangSerialization clangParse clangSema clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangAnalysis clangARCMigrate clangRewrite clangRewriteFrontend clangEdit clangAST clangLex clangBasic clang-cpp clangCodeGen /usr/lib/llvm-16/lib/libLLVM.so)

