//
// Created by jolechiw on 7/6/23.
//

#include "MainView.h"

void MainView::render() {
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
    ImGui::Button(ICON_FA_FILE);
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_SAVE)) {
        _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Debug, "Save Clicked"});
        _save = true;
    }
    ImGui::SameLine();
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
    _outputTextView.Render("Text Output");
    ImGui::EndChild();
    _diagnosticsView.render();
    ImGui::End();
}

bool MainView::isOpen() const { return _is_open; }

bool MainView::build() {
    return takeFlag(_build);
}

bool MainView::run() {
    return takeFlag(_run);
}

bool MainView::save() {
    return takeFlag(_save);
}

bool MainView::takeFlag(bool &val)  {
    if (val) {
        val = false;
        return true;
    }
    return false;
}

MainView::MainView(Diagnostics &diagnostics, FileEditorView &fileEditorView, DiagnosticsView &diagnosticsView) :
        _diagnostics(diagnostics),
        _diagnosticsView(diagnosticsView),
        _fileEditorView(fileEditorView) {
    _outputTextView.SetReadOnly(true);
}

void MainView::appendOutputText(const std::string &text) {
    _outputTextView.SetReadOnly(false);
    _outputTextView.MoveBottom();
    _outputTextView.InsertText(text);
    _outputTextView.SetReadOnly(true);
}
