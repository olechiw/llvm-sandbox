//
// Created by jolechiw on 6/28/23.
//

#include "MainView.h"

MainView::MainView(Context &context) : _context(context), _fileEditor(context) {
}

void MainView::render() {
    auto io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin("MainWindow", &_is_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Exit");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    _fileEditor.render();

    ImGui::End();
}
