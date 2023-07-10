//
// Created by jolechiw on 7/6/23.
//

#include "MainView.h"
// TODO: filters for diagnostics
// TODO: clear output window
void MainView::render() {
    auto io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin("MainWindow", &_is_open,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem(ICON_FA_DOOR_OPEN " Exit");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    _contextComboBox.render();
    ImGui::SameLine();
    newButton.render();
    ImGui::SameLine();
    saveButton.render();
    ImGui::SameLine();
    buildButton.render();
    ImGui::SameLine();
    runButton.render();


    auto regionAvailable = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("Output Child", {regionAvailable.x * 2.f / 3.f, regionAvailable.y * 2.f / 3.f});
    _fileEditorView.render();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("Other Out Child", {regionAvailable.x * 1.f / 3.f, regionAvailable.y * 2.f / 3.f});
    _outputTextView.Render("Text Output");
    ImGui::EndChild();
    _diagnosticsView.render();
    ImGui::End();
}

bool MainView::isOpen() const {
    return _is_open;
}

MainView::MainView(Diagnostics &diagnostics, FileEditorView &fileEditorView, DiagnosticsView &diagnosticsView, ComboBox &contextComboBox)
        : _diagnostics{diagnostics}, _diagnosticsView{diagnosticsView}, _fileEditorView{fileEditorView}, _contextComboBox{contextComboBox},
        runButton{ICON_FA_PLAY}, saveButton{ICON_FA_SAVE}, buildButton{ICON_FA_HAMMER}, newButton{ICON_FA_FILE}{
    _outputTextView.SetReadOnly(true);
}

void MainView::appendOutputText(const std::string &text) {
    _outputTextView.SetReadOnly(false);
    _outputTextView.MoveBottom();
    _outputTextView.InsertText(text);
    _outputTextView.SetReadOnly(true);
}

void MainView::Button::render() {
    ImGui::BeginDisabled(!_enabled);
    if (ImGui::Button(_label)) {
        _clicked = true;
    }
    ImGui::EndDisabled();
}

bool MainView::Button::clicked() {
    bool tmp = _clicked;
    _clicked = false;
    return tmp;
}

MainView::Button::Button(const char *label): _label{label}, _clicked{false} {
}

void MainView::Button::setEnabled(bool enabled) {
    _enabled = enabled;
}
