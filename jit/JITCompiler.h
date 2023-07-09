//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_JITCOMPILER_H
#define TESTPROJECT_JITCOMPILER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <tuple>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/Mangling.h>
#include <llvm/ExecutionEngine/Orc/SymbolStringPool.h>
#include <llvm/IR/Module.h>
#include <llvm/Demangle/Demangle.h>

#include "../model/Diagnostics.h"
#include "../model/FileSystem.h"
#include "CPPInterpreter.h"

class JITCompiler {
public:
    struct CompiledCode {
        friend class JITCompiler;

        std::unordered_map<std::string, void *> functions;
    private:
        CompiledCode() = default;

        std::unique_ptr<llvm::orc::LLJIT> engine;
    };

    using DynamicLibraries = std::unordered_map<std::string, void *>;

    static std::unique_ptr<CompiledCode> compile(Diagnostics &diagnostics, const FileSystem &files,
                                                 const DynamicLibraries &dynamicLibraries = DynamicLibraries());

private:
    static std::unique_ptr<CompiledCode>
    create(Diagnostics &diagnostics, CPPInterpreter::LLVMModuleAndContext llvmModuleAndContext,
           const DynamicLibraries &dynamicLibraries);
};


#endif //TESTPROJECT_JITCOMPILER_H
