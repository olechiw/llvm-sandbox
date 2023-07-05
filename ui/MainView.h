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
    MainView(Diagnostics &diagnostics, FileEditorView &fileEditorView, DiagnosticsView &diagnosticsView) :
    _diagnostics(diagnostics),
    _diagnosticsView(diagnosticsView),
    _fileEditorView(fileEditorView) {
    }

    void render() {
        auto io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin("MainWindow", &_is_open,
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_MenuBar |
                     ImGuiWindowFlags_NoTitleBar);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem(ICON_FA_DOOR_OPEN " Exit");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        // TODO: actionbar class and connect to fileeditor (pipe through context? probably just pipe through here)
        ImGui::Button(ICON_FA_FILE);
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_SAVE)) {
            _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Debug, "Save Clicked"});
            _save = true;
        }
        ImGui::SameLine();
        bool build {false}, run { false };
        if (ImGui::Button(ICON_FA_HAMMER)) {
            _build = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PLAY)) {
            _run = true;
        }


        auto regionAvailable = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("Output Child", {regionAvailable.x * 2.f/3.f, regionAvailable.y * 2.f / 3.f});
        _fileEditorView.render();
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("Other Out Child", {regionAvailable.x * 1.f/3.f, regionAvailable.y * 2.f/3.f});
        TextEditor e;
        e.Render("Test");
        ImGui::EndChild();
        _diagnosticsView.render();
        ImGui::End();
    }

    bool isOpen() const { return _is_open; }

    bool build() {
        return takeFlag(_build);
    }

    bool run() {
        return takeFlag(_run);
    }

    bool save() {
        return takeFlag(_save);
    }
private:
    bool takeFlag(bool &val) {
        if (val) {
            val = false;
            return true;
        }
        return false;
    }
    FileEditorView &_fileEditorView;
    DiagnosticsView &_diagnosticsView;
    Diagnostics &_diagnostics;
    bool _is_open { true };
    bool _build { false }, _run { false }, _save { false };
};


#endif //TESTPROJECT_MAINVIEW_H
