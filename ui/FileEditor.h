//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_FILEEDITOR_H
#define TESTPROJECT_FILEEDITOR_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <TextEditor.h>

#include "../Context.h"

class FileEditor {
public:
    FileEditor(Context &context);
    void render();
    void saveEvent();

private:
    struct FileTabState {
        bool saved = true;
        std::string name;
        File::Type type;
        TextEditor editor;
        bool isReadOnly { false };
    };

    std::unordered_map<std::string, FileTabState> _fileTabs;
    Context &_context;
    bool _saveEventPending { false };

    TextEditor getDefaultTextEditor();
    void createNewFile();
    void saveFile(const FileTabState &stateToSave);
};


#endif //TESTPROJECT_FILEEDITOR_H
