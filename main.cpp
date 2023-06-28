#include <iostream>

#include "CPPInterpreter.h"
#include "SandboxJIT.h"

void utilityFunction(int x) {
    std::cout << "HEY " << x << std::endl;
}

int main() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    constexpr auto testCodeFileName = "test.cpp";
    constexpr auto testCode = "#include \"test.h\"\n class A {public: const static int x=5;}; int test(int y) { utilityFunction(3); return 2+A::x+y; }";

    constexpr auto testHeaderFileName = "test.h";
    constexpr auto testHeader = "extern void utilityFunction(int);";

    CPPInterpreter interpreter;
    SandboxJIT jit;
    jit.registerFunctionToDyLib("utilityFunction(int)", (void*)&utilityFunction);

    interpreter.addFile(testCodeFileName, testCode);
    interpreter.addFile(testHeaderFileName, testHeader, true);

    const std::string funcName { "test(int)" };
    auto llvmModuleAndContext = interpreter.buildModule();

    auto sandboxContext = jit.create(std::move(llvmModuleAndContext));

    using TestFunction = int (*)(int);
    auto ptr = sandboxContext->functions[funcName];
    std::cout << ((TestFunction)ptr)(5) << std::endl;
}
