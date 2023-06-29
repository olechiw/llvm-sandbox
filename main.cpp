#include <iostream>

#include "jit/CPPInterpreter.h"
#include "jit/SandboxJIT.h"
#include "ui/MainWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <TextEditor.h>

/**
 * jit/ - contains all uilities for just-in-time compiling
 * model/ - data structure
 * executor/ - runs user code against "puzzles"
 * puzzles/
 * puzzles/ui/ - definitions for puzzle UI
 * puzzles/builtins/ - builtin files that user code can link against
 * ui/ - core UI elements
 */


void utilityFunction(int x) {
    std::cout << "HEY " << x << std::endl;
}

int test(int) { return 0; }

int main() {
    constexpr auto testCodeFileName = "test.cpp";
    constexpr auto testCode = "#include \"test.h\"\n class A {public: const static int x=5;}; int test(int y) { utilityFunction(3);\n return 2+A::x+y; }";

    constexpr auto testHeaderFileName = "test.h";
    constexpr auto testHeader = "extern void utilityFunction(int);";

    Context context;
    using TestFunction = decltype(test);
    context.createOrOverwriteFile({testCodeFileName, testCode, File::Type::CPP});
    context.createOrOverwriteFile({testHeaderFileName, testHeader, File::Type::H});
    context.setDynamicLibraries({{"utilityFunction(int)", (void*)utilityFunction}});
    context.executeCode();
    auto ptr = context.getFunction<TestFunction>("test(int)");
    if (ptr) {
        std::cout << ptr(5) << std::endl;
    }

    MainWindow mainWindow(context);
    mainWindow.show();
}


