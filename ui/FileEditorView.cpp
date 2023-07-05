//
// Created by jolechiw on 6/28/23.
//

#include "FileEditorView.h"

#include "fonts/IconsFontAwesome5.h"
#include "../model/FileSystem.h"

FileEditorView::FileEditorView(FileSystem &fileSystem, Diagnostics &diagnostics) : _fileSystem(fileSystem), _diagnostics(diagnostics) {
    auto fileSystemCopy = _fileSystem.getFiles();

    if (!fileSystemCopy.empty()) {
        for (auto &[name, file] : fileSystemCopy) {
            _fileTabs.insert({name, {file.metadata, getDefaultTextEditor()}});
            _fileTabs[name].editor.SetReadOnly(file.metadata.isReadOnly);
            _fileTabs[name].editor.SetText(file.contents);
        }
    }
}

void FileEditorView::render() {
    if (ImGui::BeginTabBar("Files")) {
        for (auto &[name, tabState] : _fileTabs) {
            if (tabState.editor.IsTextChanged() && tabState.hasRenderedOnce) {
                tabState.saved = false;
            }
            ImGuiTabItemFlags flags = tabState.saved ? 0 : ImGuiTabItemFlags_UnsavedDocument;
            if (ImGui::BeginTabItem(name.c_str(), nullptr, flags)) {
                // TODO: pin readonly tabs to the left
                if (_saveEventPending && !tabState.saved && !tabState.metadata.isReadOnly) {
                    saveFile(tabState);
                    _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Debug, "Saved" + name});
                    tabState.saved = true;
                }
                tabState.editor.Render(name.c_str());
                tabState.hasRenderedOnce = true;
                ImGui::EndTabItem();
            }
        }

        if (ImGui::TabItemButton("+")) {
            createNewFile();
        }
        ImGui::EndTabBar();
    }
    _saveEventPending = false;
}

TextEditor FileEditorView::getDefaultTextEditor() {
    TextEditor out;
    out.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    return out;
}

void FileEditorView::createNewFile() {

}

void FileEditorView::saveEvent() {
    _saveEventPending = true;
}

void FileEditorView::saveFile(const FileEditorView::FileTabState &stateToSave) {
    _fileSystem.createOrOverwriteFile({stateToSave.metadata, stateToSave.editor.GetText()});
}
