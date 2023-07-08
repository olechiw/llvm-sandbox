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


template <typename T>
class ExecutionContext {
public:
    static T &getInstance() {
        return _instance;
    }
    void run(JITCompiler::CompiledCode compiledCode) {
        static_cast<T*>(this)->run(compiledCode);
    }

    bool isRunning() {
        return static_cast<T*>(this)->isRunning();
    }

    void render() {
        return static_cast<T*>(this)->render();
    }

    std::vector<std::string> takeOutput() {
        return std::move(output);
    }
private:
    static T _instance;
protected:
    std::vector<std::string> output {};
};

template<typename T>
T ExecutionContext<T>::_instance;

class HelloWorldExecutionContext : public ExecutionContext<HelloWorldExecutionContext> {
public:
    static void print(const char *value) {
        getInstance().output.emplace_back(value);
    }

    const Files StarterFiles {
            {"main.cpp",
             {
                     {
                             "main.cpp",
                             File::Type::CPP,
                             false},
                     "#include \"system_headers.h\"\n\nint main() {\n\tprint(\"Hello World\\n\");\n}"}
            }
    };
    const Files HelperFiles {
            {"system_headers.h",
             {
                     {
                             "system_headers.h",
                             File::Type::H,
                             true},
                     "extern void print(const char*);"}
            }
    };
    const JITCompiler::DynamicLibraries DynamicLibraries {
            {"print(char const*)", (void*)HelloWorldExecutionContext::print}
    };

    void run(JITCompiler::CompiledCode &compiledCode) {
        using MainType = int(*)(void);
        auto function = compiledCode.functions.find("main");
        if (function != compiledCode.functions.end()) {
            ((MainType) function->second)();
        }
    }

    bool isRunning() {
        return false;
    }

    void render() {

    }
private:
};


#endif //TESTPROJECT_EXECUTIONCONTEXT_H
