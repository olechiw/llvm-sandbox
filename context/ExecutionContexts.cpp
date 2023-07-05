//
// Created by jolechiw on 7/5/23.
//

#include "ExecutionContexts.h"

void print(const char * value) {
    std::cout << value;
}

static ExecutionContexts::T helloWorldContext {
        .helperFiles = {{"system_headers.h", "extern void print(const char *);"}},
        .starterFiles = {{"main.cpp", "#include \"system_headers.h\"\nint main() {\n\tprint(\"Hello World\\n\");\n}"}},
        .dynamicLibraries = {{"print(char const*)", (void*)print}}
};

const ExecutionContexts::T &ExecutionContexts::getHelloWorld() {
    return helloWorldContext;
}
