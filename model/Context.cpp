//
// Created by jolechiw on 6/29/23.
//

#include "Context.h"

Context::Context() : _diagnosticsConsumer(), _jit(_diagnosticsConsumer) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

FileSystem Context::getFiles() {
    return _files;
}

void Context::createOrOverwriteFile(const File &file) {
    _files[file.metadata.name] = file;
}

void Context::deleteFile(const std::string &name) {
    _files.erase(name);
}

void Context::resetFiles() {
    _files.clear();
}

void Context::setFiles(const FileSystem &fs) {
    _files = fs;
}

void Context::setDynamicLibraries(const std::unordered_map<std::string, void*> &dynamicLibraries) {
    _dynamicLibraries = dynamicLibraries;
}

void Context::executeCode() {
    _jitContext = _jit.execute(_files, _dynamicLibraries);

    for (const auto &diag : _diagnosticsConsumer.take()) {
        std::cout << diag.message << std::endl;
    }
}