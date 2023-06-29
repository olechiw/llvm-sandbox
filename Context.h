//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_CONTEXT_H
#define TESTPROJECT_CONTEXT_H

#include "jit/SandboxJIT.h"
#include "FileSystem.h"

class Context {
public:
    Context() : _diagnosticsConsumer(), _jit(_diagnosticsConsumer) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
    }

    FileSystem getFiles() {
        return _files;
    }

    void createOrOverwriteFile(const File &file) {
        _files[file.name] = file;
    }

    void deleteFile(const std::string &name) {
        _files.erase(name);
    }

    void resetFiles() {
        _files.clear();
    }

    void setFiles(const FileSystem &fs) {
        _files = fs;
    }

    void setDynamicLibraries(const std::unordered_map<std::string, void*> &dynamicLibraries) {
        _dynamicLibraries = dynamicLibraries;
    }

    void executeCode() {
        _jitContext = _jit.execute(_files, _dynamicLibraries);

        for (const auto &diag : _diagnosticsConsumer.take()) {
            std::cout << diag.message << std::endl;
        }
    }

    template <typename T>
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
    SandboxJIT _jit;
    std::unordered_map<std::string, void*> _dynamicLibraries;
    DiagnosticsConsumer _diagnosticsConsumer;
    FileSystem _files;
    std::unique_ptr<SandboxJIT::JITContext> _jitContext;
};


#endif //TESTPROJECT_CONTEXT_H
