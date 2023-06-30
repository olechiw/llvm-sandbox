//
// Created by jolechiw on 6/28/23.
//

#include "MainView.h"

#include "fonts/IconsFontAwesome5.h"

MainView::MainView(Context &context) : _context(context), _fileEditor(context), _diagnosticsView(context) {
}

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
    // TODO: actionbar class and connect to fileeditor (pipe through context? probably just pipe through here)
    ImGui::Button(ICON_FA_FILE);
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_SAVE)) {
        _context.getDiagnosticsConsumer().push({DiagnosticsConsumer::Type::System, DiagnosticsConsumer::Level::Debug, "Save Clicked"});
        _fileEditor.saveEvent();
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PLAY)) {
        _context.buildCode();
    }


    auto regionAvailable = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("Output Child", {regionAvailable.x * 2.f/3.f, regionAvailable.y * 2.f / 3.f});
    _fileEditor.render();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("Other Out Child", {regionAvailable.x * 1.f/3.f, regionAvailable.y * 2.f/3.f});
    TextEditor e;
    e.Render("Test");
    ImGui::EndChild();
    _diagnosticsView.render();
    ImGui::End();
}
