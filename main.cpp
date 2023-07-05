#include <iostream>

#include "jit/CPPInterpreter.h"
#include "jit/JITCompiler.h"
#include "ui/MainWindow.h"
#include "CodeActions.h"
#include "context/ExecutionContexts.h"

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
    FileSystem fs;
    Diagnostics diagnostics;
    CodeActions codeActions(fs, diagnostics);
    for (const auto &[name, contents] : ExecutionContexts::getHelloWorld().starterFiles) {
        fs.createOrOverwriteFile({{name, File::Type::CPP, false}, contents});
    }
    for (const auto &[name, contents] : ExecutionContexts::getHelloWorld().helperFiles) {
        fs.createOrOverwriteFile({{name, File::Type::H, true}, contents});
    }
    MainWindow mainWindow(fs, diagnostics, codeActions);
    mainWindow.show();
}


