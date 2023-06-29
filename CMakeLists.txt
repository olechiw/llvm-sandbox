cmake_minimum_required(VERSION 3.10.0)
project(TestProject)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# C++ Preferences
set(CMAKE_CXX_STANDARD 20) # C++20...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(llvm-sandbox main.cpp CPPInterpreter.cpp CPPInterpreter.h SandboxJIT.cpp SandboxJIT.h
        imgui/imgui.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp
        imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)
# Link against LLVM libraries
target_include_directories(llvm-sandbox PUBLIC "${CMAKE_SOURCE_DIR}/imgui")
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
target_link_libraries(llvm-sandbox ${SDL2_LIBRARIES})
target_link_libraries(llvm-sandbox GL)