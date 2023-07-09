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
    explicit FileEditorView(Diagnostics &diagnostics);

    void render();

    void saveCurrentFile();

    Files takeChangedFiles();

    void setFileSystem(const FileSystem &fileSystem);

private:
    struct FileTabState {
        File::Metadata metadata;
        TextEditor editor;
        bool saved{true};
        bool hasRenderedOnce{false};
    };
    bool _saveCurrentFile{false};

    std::unordered_map<std::string, FileTabState> _fileTabs;
    Files _changedFiles;
    Diagnostics &_diagnostics;

    static TextEditor getDefaultTextEditor();

    void createNewFile();
};


#endif //TESTPROJECT_FILEEDITORVIEW_H
