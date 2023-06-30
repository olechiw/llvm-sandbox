//
// Created by jolechiw on 6/28/23.
//

#include "FileEditor.h"

#include "fonts/IconsFontAwesome5.h"

FileEditor::FileEditor(Context &context) : _context(context) {
    auto fileSystemCopy = _context.getFiles();

    if (!fileSystemCopy.empty()) {
        for (auto &[name, file] : fileSystemCopy) {
            _fileTabs.insert({name, {file.metadata, getDefaultTextEditor()}});
            _fileTabs[name].editor.SetReadOnly(file.metadata.isReadOnly);
            _fileTabs[name].editor.SetText(file.contents);
        }
    }
}

void FileEditor::render() {
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
                    _context.getDiagnosticsConsumer().push({DiagnosticsConsumer::Type::System, DiagnosticsConsumer::Level::Debug, "Saved" + name});
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

TextEditor FileEditor::getDefaultTextEditor() {
    TextEditor out;
    out.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    return out;
}

void FileEditor::createNewFile() {

}

void FileEditor::saveEvent() {
    _saveEventPending = true;
}

void FileEditor::saveFile(const FileEditor::FileTabState &stateToSave) {
    _context.createOrOverwriteFile({stateToSave.metadata, stateToSave.editor.GetText()});
}
