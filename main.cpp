#include <iostream>

#include "jit/CPPInterpreter.h"
#include "jit/JITCompiler.h"
#include "ui/MainWindow.hpp"
#include "execution/CodeActions.h"

int main() {
    Diagnostics diagnostics;
    CodeActionsSwitcher codeActionsSwitcher(diagnostics);
    ComboBox contextComboBox("Context Combo Box", codeActionsSwitcher.getNames());
    contextComboBox.setCurrentOption(codeActionsSwitcher.getActive());

    FileEditorView fileEditorView(diagnostics);
    DiagnosticsView diagnosticsView(diagnostics);
    MainView mainView(diagnostics, fileEditorView, diagnosticsView, contextComboBox);

    codeActionsSwitcher.visit([&](auto &codeActions) {
        fileEditorView.setFileSystem(codeActions.getFileSystem());
    });
    codeActionsSwitcher.setActive(CodeActions<TestFinanceToy>::Name);

    auto eventLoop = [&]() {
        if (codeActionsSwitcher.getActive() != contextComboBox.getCurrentOption()) {
            codeActionsSwitcher.setActive(contextComboBox.getCurrentOption());
            codeActionsSwitcher.visit([&](auto &codeActions) {
                fileEditorView.setFileSystem(codeActions.getFileSystem());
            });
        }
        codeActionsSwitcher.visit([&](auto &codeActions) {
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


    MainWindow<decltype(eventLoop)> mainWindow(mainView, eventLoop);
    mainWindow.show();
}


