//
// Created by jolechiw on 6/28/23.
//

#include "MainView.h"

MainView::MainView(Context &context) : _context(context) {
    _editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
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

    if (ImGui::BeginTabBar("Files")) {
        if (ImGui::BeginTabItem("main.cpp")) {
            _editor.Render("TestEditor");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("fps")) {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::EndTabItem();
        }
        ImGui::TabItemButton("+");
        ImGui::EndTabBar();
    }

    ImGui::End();
}
