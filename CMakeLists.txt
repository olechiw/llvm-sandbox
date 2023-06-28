cmake_minimum_required(VERSION 3.10.0)
project(TestProject)

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
set(CMAKE_CXX_STANDARD 20) # C++20...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(llvm-sandbox main.cpp CPPInterpreter.cpp CPPInterpreter.h SandboxJIT.cpp SandboxJIT.h)
# Link against LLVM libraries
target_link_libraries(llvm-sandbox
        clangBasic
        clangDriver
        clangFrontend
        clangTooling
        clangFrontendTool
        clangSerialization
        clangParse
        clangSema
        clangStaticAnalyzerFrontend
        clangStaticAnalyzerCheckers
        clangStaticAnalyzerCore
        clangAnalysis
        clangARCMigrate
        clangRewrite
        clangRewriteFrontend
        clangEdit
        clangAST
        clangLex
        clangBasic
        clang-cpp
        clangCodeGen
        LLVM)

