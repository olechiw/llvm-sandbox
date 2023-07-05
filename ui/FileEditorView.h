//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_FILEEDITORVIEW_H
#define TESTPROJECT_FILEEDITORVIEW_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <TextEditor.h>

#include "../model/FileSystem.h"
#include "../model/Diagnostics.h"

class FileEditorView {
public:
    explicit FileEditorView(FileSystem &fileSystem, Diagnostics &diagnostics);
    void render();
    void saveEvent();

private:
    struct FileTabState {
        File::Metadata metadata;
        TextEditor editor;
        bool saved { true };
        bool hasRenderedOnce { false };
    };

    std::unordered_map<std::string, FileTabState> _fileTabs;
    FileSystem &_fileSystem;
    Diagnostics &_diagnostics;
    bool _saveEventPending { false };

    static TextEditor getDefaultTextEditor();
    void createNewFile();
    void saveFile(const FileTabState &stateToSave);
};


#endif //TESTPROJECT_FILEEDITORVIEW_H
