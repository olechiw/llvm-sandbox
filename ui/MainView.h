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
#include "../CodeActions.h"
#include "FileEditorView.h"
#include "DiagnosticsView.h"
#include "fonts/IconsFontAwesome5.h"


class MainView {
public:
    MainView(Diagnostics &diagnostics, FileEditorView &fileEditorView, DiagnosticsView &diagnosticsView);

    void appendOutputText(const std::string &text);

    void render();

    bool isOpen() const;

    bool build();

    bool run();

    bool save();
private:
    static bool takeFlag(bool &val);
    FileEditorView &_fileEditorView;
    DiagnosticsView &_diagnosticsView;
    TextEditor _outputTextView;
    Diagnostics &_diagnostics;
    bool _is_open { true };
    bool _build { false }, _run { false }, _save { false };
};


#endif //TESTPROJECT_MAINVIEW_H
