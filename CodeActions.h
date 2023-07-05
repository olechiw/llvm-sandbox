//
// Created by jolechiw on 7/5/23.
//

#ifndef TESTPROJECT_CODEACTIONS_H
#define TESTPROJECT_CODEACTIONS_H

#include "jit/JITCompiler.h"

#include "model/Diagnostics.h"
#include "model/FileSystem.h"

class CodeActions {
public:
    explicit CodeActions(const FileSystem &fileSystem, Diagnostics &diagnostics);
    void build(const JITCompiler::DynamicLibraries &dynamicLibraries);
    void runBuiltCode();
private:
    Diagnostics &_diagnostics;
    const FileSystem &_fileSystem;
    std::unique_ptr<JITCompiler::CompiledCode> _compiledCode { nullptr };
};


#endif //TESTPROJECT_CODEACTIONS_H
