cmake_minimum_required(VERSION 3.10.0)
project(TestProject)

# Hacking this in here for my local installation
SET(LLVM_DIR /usr/local/clang_16.0.0/lib/cmake/llvm)
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
#add_executable(simple-tool test.cpp)

set(CMAKE_CXX_STANDARD 17) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(simple-tool main.cpp)
# Find the libraries that correspond to the LLVM components
# that we wish to use
 llvm_map_components_to_libnames(llvm_libs ${LLVM_TARGETS_TO_BUILD} support core irreader codegen target)
# Link against LLVM libraries
target_link_libraries(simple-tool ${llvm_libs})


#cmake_minimum_required(VERSION 3.10)
#project(project_name)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#
##set(CMAKE_C_COMPILER /usr/local/clang_16.0.0/bin/clang)
##set(CMAKE_CXX_COMPILER /usr/local/clang_16.0.0/bin/clang++)
#
#link_libraries()
#execute_process(COMMAND /usr/local/clang_16.0.0/bin/llvm-config --libs OUTPUT_VARIABLE LIBS)
#execute_process(COMMAND /usr/local/clang_16.0.0/bin/llvm-config --system-libs OUTPUT_VARIABLE SYS_LIBS)
#execute_process(COMMAND /usr/local/clang_16.0.0/bin/llvm-config --ldflags OUTPUT_VARIABLE LDF)
#message(STATUS "Found LLVM" ${LDF})
#
#string(STRIP ${LIBS} LIBS)
#string(STRIP ${SYS_LIBS} SYS_LIBS)
#string(STRIP ${LDF} LDF)
#
#link_libraries(${LIBS} ${SYS_LIBS} ${LDF})
#
#execute_process(COMMAND /usr/local/clang_16.0.0/bin/llvm-config --cxxflags OUTPUT_VARIABLE CMAKE_CXX_FLAGS)
#string(STRIP ${CMAKE_CXX_FLAGS} CMAKE_CXX_FLAGS)
#
#add_executable(project_name main.cpp)