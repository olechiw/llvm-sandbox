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

#include "../model/DiagnosticsConsumer.h"
#include "../model/Context.h"
#include "FileEditor.h"
#include "DiagnosticsView.h"

class MainView {
public:
    MainView(Context &context);
    void render();
    bool isOpen() const { return _is_open; }
private:
    FileEditor _fileEditor;
    DiagnosticsView _diagnosticsView;
    Context &_context;
    bool _is_open { true };
};


#endif //TESTPROJECT_MAINVIEW_H
