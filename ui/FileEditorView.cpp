//
// Created by jolechiw on 6/28/23.
//

#include "FileEditorView.h"

#include "fonts/IconsFontAwesome5.h"
#include "../model/FileSystem.h"

FileEditorView::FileEditorView(Diagnostics &diagnostics) : _diagnostics(diagnostics) {
}

void FileEditorView::render() {
    if (ImGui::BeginTabBar("Files")) {
        for (auto &[name, tabState]: _fileTabs) {
            if (tabState.editor.IsTextChanged() && tabState.hasRenderedOnce) {
                tabState.saved = false;
            }
            ImGuiTabItemFlags flags = tabState.saved ? 0 : ImGuiTabItemFlags_UnsavedDocument;
//            flags |= tabState.metadata.isReadOnly ? ImGuiTabItemFlags_Trailing : 0;
            if (ImGui::BeginTabItem(name.c_str(), nullptr, flags)) {
                tabState.editor.Render(name.c_str());
                tabState.hasRenderedOnce = true;
                if (_saveCurrentFile) {
                    if (!tabState.saved) {
                        _changedFiles.insert({tabState.metadata.name, {tabState.metadata, tabState.editor.GetText()}});
                        tabState.saved = true;
                    }
                    _saveCurrentFile = false;
                }
                ImGui::EndTabItem();
            }
        }

        if (ImGui::TabItemButton("+")) {
            createNewFile();
        }
        ImGui::EndTabBar();
    }
}

TextEditor FileEditorView::getDefaultTextEditor() {
    TextEditor out;
    out.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    return out;
}

void FileEditorView::setFileSystem(const FileSystem &fileSystem) {
    _fileTabs.clear();
    auto &files = fileSystem.getFiles();
    if (!files.empty()) {
        for (auto &[name, file]: files) {
            _fileTabs.insert({name, {file.metadata, getDefaultTextEditor()}});
            _fileTabs[name].editor.SetReadOnly(file.metadata.isReadOnly);
            _fileTabs[name].editor.SetText(file.contents);
        }
    }
}

Files FileEditorView::takeChangedFiles() {
    return std::move(_changedFiles);
}

void FileEditorView::saveCurrentFile() {
    _saveCurrentFile = true;
}

void FileEditorView::createNewFile() {

}
