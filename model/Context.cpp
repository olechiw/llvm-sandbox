//
// Created by jolechiw on 6/29/23.
//

#include "Context.h"

Context::Context() : _diagnosticsConsumer() {
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

void Context::buildCode() {
    _diagnosticsConsumer.push({DiagnosticsConsumer::Type::System, DiagnosticsConsumer::Level::Info, "Compiling..."});
    auto context = JITExecutor::execute(_diagnosticsConsumer, _files, _dynamicLibraries);
    if (context)
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::System, DiagnosticsConsumer::Level::Info, "Compiled"});
    else
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::System, DiagnosticsConsumer::Level::Info, "Failed to Compile"});
    _lockedJITContext.set(std::move(context));
}

DiagnosticsConsumer &Context::getDiagnosticsConsumer() {
    return _diagnosticsConsumer;
}

std::unique_ptr<JITExecutor::JITContext> Context::takeJITContext() {
    return _lockedJITContext.take();
}

void Context::LockedJITContext::set(std::unique_ptr<JITExecutor::JITContext> newContext) {
    std::unique_lock<std::mutex> l(_lock);
    _jitContext = std::move(newContext);
}

std::unique_ptr<JITExecutor::JITContext> Context::LockedJITContext::take() {
    std::unique_lock<std::mutex> l(_lock);
    return std::move(_jitContext);
}
