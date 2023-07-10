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

set(IMGUI_DIR dependencies/imgui)
set(TEXTEDIT_DIR dependencies/ImGuiColorTextEdit)
add_executable(llvm-sandbox main.cpp jit/CPPInterpreter.cpp jit/CPPInterpreter.h jit/JITCompiler.cpp jit/JITCompiler.h
        ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp
        ${TEXTEDIT_DIR}/TextEditor.cpp ${TEXTEDIT_DIR}/LanguageDefinitions.cpp ui/MainWindow.hpp ui/MainView.h model/Diagnostics.h model/FileSystem.h ui/FileEditorView.cpp ui/FileEditorView.h model/Diagnostics.cpp ui/fonts/Loader.h ui/fonts/IconsFontAwesome5.h ui/fonts/IconsFontAwesome5.h_fa-regular-400.ttf.h ui/fonts/IconsFontAwesome5.h_fa-solid-900.ttf.h ui/DiagnosticsView.cpp ui/DiagnosticsView.h model/FileSystem.cpp execution/ContextRunner.hpp execution/context/ExecutionContext.h ui/MainView.cpp model/InterprocessQueue.hpp execution/SubProcess.hpp execution/context/FinanceToyContexts.h ui/ComboBox.cpp ui/ComboBox.h)
target_include_directories(llvm-sandbox PUBLIC "${CMAKE_SOURCE_DIR}/${IMGUI_DIR}"
        PUBLIC "${CMAKE_SOURCE_DIR}/${TEXTEDIT_DIR}/vendor/regex/include"
        PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/lockfree/include"
        PUBLIC "${CMAKE_SOURCE_DIR}/${TEXTEDIT_DIR}")

# Link against clang
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