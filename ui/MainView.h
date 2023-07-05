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

class MainView {
public:
    MainView(FileSystem &fileSystemStore, Diagnostics &diagnostics, CodeActions &actions);
    void render();
    bool isOpen() const { return _is_open; }
private:
    FileEditorView _fileEditor;
    DiagnosticsView _diagnosticsView;
    Diagnostics &_diagnostics;
    CodeActions &_codeActions;
    bool _is_open { true };
};


#endif //TESTPROJECT_MAINVIEW_H
