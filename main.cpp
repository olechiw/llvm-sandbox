#include <iostream>

#include "CPPInterpreter.h"

int main() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    constexpr auto testCodeFileName = "test.cpp";
    constexpr auto testCode = "extern void utilityFunction(int); class A {public: const static int x=5;}; int test(int y) { utilityFunction(3); return 2+A::x+y; }";

    CPPInterpreter interpreter;

    interpreter.addFile(testCodeFileName, testCode);

    const std::string funcName { "test(int)" };
    auto context = interpreter.compile({ funcName });

    using TestFunction = int (*)(int);
    auto ptr = context->functions[funcName];
    std::cout << ((TestFunction)ptr)(5) << std::endl;
}
