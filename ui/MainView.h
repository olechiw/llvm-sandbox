//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_MAINVIEW_H
#define TESTPROJECT_MAINVIEW_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <TextEditor.h>

#include "../model/Diagnostics.h"
#include "../model/FileSystem.h"
#include "FileEditorView.h"
#include "DiagnosticsView.h"
#include "fonts/IconsFontAwesome5.h"
#include "ComboBox.h"


class MainView {
public:
    class Button;

    MainView(Diagnostics &diagnostics, FileEditorView &fileEditorView, DiagnosticsView &diagnosticsView,
             ComboBox &contextComboBox);

    void appendOutputText(const std::string &text);

    void render();

    bool isOpen() const;

    class Button {
    public:
        bool clicked();

        void setEnabled(bool enabled);

    private:
        const char *_label;
        bool _enabled{true};
        bool _clicked{false};

        void render();

        Button(const char *label);

        friend class MainView;
    };

    Button runButton, saveButton, buildButton, newButton;

private:
    FileEditorView &_fileEditorView;
    DiagnosticsView &_diagnosticsView;
    TextEditor _outputTextView;
    Diagnostics &_diagnostics;
    ComboBox &_contextComboBox;
    bool _is_open{true};
};


#endif //TESTPROJECT_MAINVIEW_H
