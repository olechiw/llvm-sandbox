//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_MAINWINDOW_H
#define TESTPROJECT_MAINWINDOW_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>

#include "MainView.h"

class MainWindow {
public:
    static constexpr float fontSize = 13.f;
    MainWindow(Context &context);
    ~MainWindow();
    void show();
private:
    SDL_Window *_window;
    void *_glContext;
    MainView _mainView;
};


#endif //TESTPROJECT_MAINWINDOW_H
