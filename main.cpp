#include <iostream>

#include "jit/CPPInterpreter.h"
#include "jit/JITCompiler.h"
#include "ui/MainWindow.hpp"
#include "execution/CodeActions.h"

int main() {
    Diagnostics diagnostics;
    CodeActionsSwitcher codeActions(diagnostics);

    FileEditorView fileEditorView(diagnostics);
    DiagnosticsView diagnosticsView(diagnostics);
    MainView mainView(diagnostics, fileEditorView, diagnosticsView);

    codeActions.visit([&](auto &codeActions) {
        fileEditorView.setFileSystem(codeActions.getFileSystem());
    });
    codeActions.setActive(CodeActions<TestFinanceToy>::Name);

    auto processEvents = [&](MainView &mainView) {
        codeActions.visit([&](auto &codeActions) {
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
            for (const auto &[fileName, file]: fileEditorView.takeChangedFiles()) {
                codeActions.getFileSystem().createOrOverwriteFile(file);
            }
            for (const auto &line: codeActions.takeOutput()) {
                mainView.appendOutputText(line);
            }
        });
    };


    MainWindow<decltype(processEvents)> mainWindow(mainView, diagnostics, processEvents);
    mainWindow.show();
}


