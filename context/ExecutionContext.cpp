//
// Created by jolechiw on 7/5/23.
//

#include "ExecutionContext.h"

const Files HelloWorldExecutionContext::HelperFiles {
    {"system_headers.h",
     {
        {
            "system_headers.h",
            File::Type::H,
            true},
      "extern void print(const char *);"}
    }
};

const Files HelloWorldExecutionContext::StarterFiles {
        {"main.cpp",
         {
                 {
                         "main.cpp",
                         File::Type::CPP,
                         false},
                 "#include \"system_headers.h\"\n\nint main() {\n\tprint(\"Hello World\\n\");\n}"}
        }
};

const JITCompiler::DynamicLibraries HelloWorldExecutionContext::DynamicLibraries {
        {"print(char const*)", (void*)HelloWorldExecutionContext::print}
};

std::vector<std::string> HelloWorldExecutionContext::output {};

void HelloWorldExecutionContext::run(std::unique_ptr<JITCompiler::CompiledCode> compiledCode) {
    using MainType = int(*)(void);
    if (compiledCode) {
        auto function = compiledCode->functions.find("main");
        if (function != compiledCode->functions.end()) {
            ((MainType)function->second)();
        }
    }
}

void HelloWorldExecutionContext::print(const char *value) {
    output.emplace_back(value);
}

std::vector<std::string> HelloWorldExecutionContext::takeOutput() {
    return std::move(output);
}

bool HelloWorldExecutionContext::isRunning() {
    return false;
}

void HelloWorldExecutionContext::render() {
    // TODO: stuff
}
