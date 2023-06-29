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
    void executeCode();

    template <typename T = void*>
    T *getFunction(const std::string &functionName) {
        if (_jitContext)
            return (T*)_jitContext->functions[functionName];
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::User,
                                   DiagnosticsConsumer::Level::Warning,
                                   "Failed to resolve function",
                                   functionName});
        return nullptr;
    }
private:
    JITExecutor _jit;
    std::unordered_map<std::string, void*> _dynamicLibraries;
    DiagnosticsConsumer _diagnosticsConsumer;
    FileSystem _files;
    std::unique_ptr<JITExecutor::JITContext> _jitContext;
};


#endif //TESTPROJECT_CONTEXT_H
