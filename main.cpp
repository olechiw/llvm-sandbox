#include <iostream>

#include "jit/CPPInterpreter.h"
#include "jit/JITCompiler.h"
#include "ui/MainWindow.hpp"
#include "execution/ContextRunner.hpp"

int main() {
    Diagnostics diagnostics;
    ContextSwitcher contextSwitcher(diagnostics);
    contextSwitcher.setActive(ContextRunner<TestFinanceToy>::Name);

    ComboBox contextComboBox("##Context Combo Box", "Context", contextSwitcher.getNames());
    contextComboBox.setCurrentOption(contextSwitcher.getActive());

    FileEditorView fileEditorView(diagnostics);
    DiagnosticsView diagnosticsView(diagnostics);
    MainView mainView(diagnostics, fileEditorView, diagnosticsView, contextComboBox);

    contextSwitcher.visit([&](auto &context) {
        fileEditorView.setFileSystem(context.getFileSystem());
    });

    auto eventLoop = [&]() {
        if (contextSwitcher.getActive() != contextComboBox.getCurrentOption()) {
            contextSwitcher.setActive(contextComboBox.getCurrentOption());
            contextSwitcher.visit([&](auto &context) {
                fileEditorView.setFileSystem(context.getFileSystem());
            });
        }
        contextSwitcher.visit([&](auto &context) {
            if (mainView.saveButton.clicked()) {
                fileEditorView.saveCurrentFile();
            }
            if (mainView.buildButton.clicked()) {
                context.build();
            }
            mainView.runButton.setEnabled(context.isBuilt() && !context.isRunning());
            if (mainView.runButton.clicked()) {
                context.runBuiltCode();
            }
            context.render();
            for (const auto &[fileName, file]: fileEditorView.takeChangedFiles()) {
                context.getFileSystem().createOrOverwriteFile(file);
            }
            for (const auto &line: context.takeOutput()) {
                mainView.appendOutputText(line);
            }
        });
    };


    MainWindow<decltype(eventLoop)> mainWindow(mainView, eventLoop);
    mainWindow.show();
}


