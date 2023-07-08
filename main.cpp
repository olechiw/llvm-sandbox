#include <iostream>

#include "jit/CPPInterpreter.h"
#include "jit/JITCompiler.h"
#include "ui/MainWindow.hpp"
#include "CodeActions.h"
#include "context/ExecutionContext.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <TextEditor.h>


int main() {
    Diagnostics diagnostics;
    FinanceToyActions codeActions(diagnostics);

    FileEditorView fileEditorView(codeActions.getFileSystem(), diagnostics);
    DiagnosticsView diagnosticsView(diagnostics);
    MainView mainView(diagnostics, fileEditorView, diagnosticsView);

    auto processEvents = [&](MainView &mainView) {
        if (mainView.save()) {
            fileEditorView.saveCurrentFile();
        }
        if (mainView.build()) {
            codeActions.build();
        }
        if (mainView.run()) {
            codeActions.runBuiltCode();
        }
        codeActions.render();
        for (const auto &[fileName, file] : fileEditorView.takeChangedFiles()) {
            codeActions.getFileSystem().createOrOverwriteFile(file);
        }
        for (const auto &line : codeActions.takeOutput()) {
            mainView.appendOutputText(line);
        }
    };


    MainWindow<decltype(processEvents)> mainWindow(mainView, diagnostics, processEvents);
    mainWindow.show();
}


