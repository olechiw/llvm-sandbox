//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_CONTEXT_H
#define TESTPROJECT_CONTEXT_H

#include "../jit/JITExecutor.h"
#include "FileSystem.h"

class Context {
public:
    Context();

    FileSystem getFiles();
    void deleteFile(const std::string &name);
    void resetFiles();
    void setFiles(const FileSystem &fs);
    void createOrOverwriteFile(const File &file);

    void setDynamicLibraries(const std::unordered_map<std::string, void *> &dynamicLibraries);
    void buildCode();

    std::unique_ptr<JITExecutor::JITContext> takeJITContext();

    DiagnosticsConsumer &getDiagnosticsConsumer();
private:
    std::unordered_map<std::string, void*> _dynamicLibraries;
    DiagnosticsConsumer _diagnosticsConsumer;
    FileSystem _files;
    struct LockedJITContext {
        void set(std::unique_ptr<JITExecutor::JITContext> newContext);

        std::unique_ptr<JITExecutor::JITContext> take();
    private:
        std::mutex _lock;
        std::unique_ptr<JITExecutor::JITContext> _jitContext;
    } _lockedJITContext;
};


#endif //TESTPROJECT_CONTEXT_H
