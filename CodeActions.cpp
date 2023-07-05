//
// Created by jolechiw on 7/5/23.
//

#include "CodeActions.h"

void CodeActions::build(const JITCompiler::DynamicLibraries &libraries) {
    _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Info, "Compiling..."});
    _compiledCode = JITCompiler::compile(_diagnostics, _fileSystem, libraries);
    const auto result = _compiledCode ? "Compiled" : "Failed to Compile";
    _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Info, result});
}

void CodeActions::runBuiltCode() {
    using MainType = int(*)(void);
    if (_compiledCode) {
        auto function = _compiledCode->functions.find("main");
        if (function != _compiledCode->functions.end()) {
            ((MainType)function->second)();
        }
    }
}

CodeActions::CodeActions(const FileSystem &fileSystem, Diagnostics &diagnostics) : _fileSystem(fileSystem), _diagnostics(diagnostics) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}
