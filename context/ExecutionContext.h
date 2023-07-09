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
class ExecutionContext {
public:
    static T &getInstance() {
        return _instance;
    }

    std::vector<std::string> takeOutput() {
        return std::move(output);
    }

private:
    static T _instance;
protected:
    std::vector<std::string> output{};
    ExecutionContext() = default;
};

template<typename T> T ExecutionContext<T>::_instance;

template<typename T> concept IsExecutionContext = requires {
    std::is_base_of_v<ExecutionContext<T>, T> &&
            std::is_invocable_v<decltype(&T::run), T&, JITCompiler::CompiledCode&> &&
            std::is_invocable_r_v<bool, decltype(&T::isRunning), T&> &&
            std::is_same_v<decltype(T::getInstance().StarterFiles), const Files> &&
            std::is_same_v<decltype(T::getInstance().HelperFiles), const Files> &&
            std::is_same_v<decltype(T::getInstance().DynamicLibraries), const JITCompiler::DynamicLibraries>;
};

class HelloWorldExecutionContext : public ExecutionContext<HelloWorldExecutionContext> {
public:
    static void print(const char *value) {
        getInstance().output.emplace_back(value);
    }

    const Files StarterFiles{{"main.cpp", {{"main.cpp", File::Type::CPP, false},
                                           "#include \"system_headers.h\"\n\nint main() {\n\tprint(\"Hello World\\n\");\n}"}}};
    const Files HelperFiles{
            {"system_headers.h", {{"system_headers.h", File::Type::H, true}, "extern void print(const char*);"}}};
    const JITCompiler::DynamicLibraries DynamicLibraries{
            {"print(char const*)", (void *) HelloWorldExecutionContext::print}};

    void run(JITCompiler::CompiledCode &compiledCode) {
        using MainType = int (*)(void);
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
};


#endif //TESTPROJECT_EXECUTIONCONTEXT_H
