//
// Created by jolechiw on 6/28/23.
//

#ifndef TESTPROJECT_SANDBOXJIT_H
#define TESTPROJECT_SANDBOXJIT_H

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


class SandboxJIT {
public:
    using LLVMModuleAndContext = std::tuple<std::unique_ptr<llvm::Module>, std::unique_ptr<llvm::LLVMContext>>;

    struct JITContext {
        friend class SandboxJIT;
        std::unordered_map<std::string, void *> functions;
    private:
        JITContext() = default;
        std::unique_ptr<llvm::orc::LLJIT> engine;
    };

    std::unique_ptr<JITContext> create(LLVMModuleAndContext llvmModuleAndContext);
    void registerFunctionToDyLib(const std::string &symbol, void *address);
private:
    std::unordered_map<std::string, void*> _registeredFunctions;
};


#endif //TESTPROJECT_SANDBOXJIT_H
