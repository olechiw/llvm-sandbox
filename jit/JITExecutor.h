//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_JITEXECUTOR_H
#define TESTPROJECT_JITEXECUTOR_H

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

#include "../model/DiagnosticsConsumer.h"
#include "../model/FileSystem.h"
#include "CPPInterpreter.h"

class JITExecutor {
public:
    struct JITContext {
        friend class JITExecutor;
        std::unordered_map<std::string, void *> functions;
    private:
        JITContext() = default;
        std::unique_ptr<llvm::orc::LLJIT> engine;
    };

    using DynamicLibraries = std::unordered_map<std::string, void*>;
    using FileSystem = std::unordered_map<std::string, File>;
    static std::unique_ptr<JITContext> execute(DiagnosticsConsumer &diagnosticsConsumer, const FileSystem &files, const DynamicLibraries &dynamicLibraries = DynamicLibraries());
private:
    static std::unique_ptr<JITContext> create(DiagnosticsConsumer &diagnosticsConsumer, CPPInterpreter::LLVMModuleAndContext llvmModuleAndContext, const DynamicLibraries &dynamicLibraries);
};


#endif //TESTPROJECT_JITEXECUTOR_H
