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

#include "../DiagnosticsConsumer.h"
#include "../Context.h"
#include "FileEditor.h"

class MainView {
public:
    MainView(Context &context);
    void render();
    bool isOpen() const { return _is_open; }
private:
    // DiagnosticsConsumer
    // CPPInterpreter
    // JITExecutor
    // EditorView
    // OutputView
    // will files have a model?
    FileEditor _fileEditor;
    Context &_context;
    bool _is_open { true };
};


#endif //TESTPROJECT_MAINVIEW_H
