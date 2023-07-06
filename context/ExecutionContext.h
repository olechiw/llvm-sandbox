//
// Created by jolechiw on 7/5/23.
//

#ifndef TESTPROJECT_EXECUTIONCONTEXT_H
#define TESTPROJECT_EXECUTIONCONTEXT_H

#include <string>
#include <unordered_map>
#include <iostream>

#include "../model/FileSystem.h"
#include "../jit/JITCompiler.h"


template<typename T>
struct ExecutionContextTraits {
    constexpr static bool Valid { false };
};

class HelloWorldExecutionContext {
public:
    const static Files StarterFiles;
    const static Files HelperFiles;
    const static JITCompiler::DynamicLibraries DynamicLibraries;

    static void run(JITCompiler::CompiledCode &compiledCode);
    static std::vector<std::string> takeOutput();
    static void print(const char *);
    static bool isRunning();
    static void render();
private:
    static std::vector<std::string> output;
};

template<>
struct ExecutionContextTraits<HelloWorldExecutionContext> {
    constexpr static bool Valid { true };
};


#endif //TESTPROJECT_EXECUTIONCONTEXT_H
