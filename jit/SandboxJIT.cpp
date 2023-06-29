//
// Created by jolechiw on 6/28/23.
//

#include "SandboxJIT.h"

std::unique_ptr<SandboxJIT::JITContext> SandboxJIT::create(SandboxJIT::LLVMModuleAndContext llvmModuleAndContext) {
    std::unique_ptr<JITContext> out(new JITContext);

    auto [module, context] = std::move(llvmModuleAndContext);

    std::unordered_map<std::string, std::string> mangledMapping;
    for (const auto &function  : module->getFunctionList()) {
        const auto demangled = llvm::demangle(function.getName().str());
        mangledMapping[demangled] = function.getName().str();
    }

    auto jitEngineExpected = llvm::orc::LLJITBuilder().create();

    if (!jitEngineExpected) {
        // TODO: Error handling
        return nullptr;
    }
    auto jitEngine = std::move(jitEngineExpected.get());
    auto &dyLib = jitEngine->getMainJITDylib();
    auto symbolPool = jitEngine->getExecutionSession().getSymbolStringPool();
    llvm::DenseMap<llvm::orc::SymbolStringPtr, llvm::JITEvaluatedSymbol> symbolsToRegister;
    for (const auto &[symbolName, addr] : _registeredFunctions) {
        // Only intern the symbols that are used
        if (mangledMapping.find(symbolName) == mangledMapping.end())
            continue;
        symbolsToRegister.insert({symbolPool->intern(mangledMapping[symbolName]),
                                  llvm::JITEvaluatedSymbol::fromPointer(addr)});
    }
    auto err = dyLib.define(llvm::orc::absoluteSymbols(symbolsToRegister));

    if (err) {
        // TODO: Error Handling
        return nullptr;
    }
    err = jitEngine->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)));
    if (err) {
        // TODO: Error handling
        return nullptr;
    }

    for (const auto &[targetName, mangledName] : mangledMapping) {
        // We are providing the mangled name because it was mangled by the frontend (clang)
        auto value = jitEngine->lookup(mangledName);
        if (value) {
            out->functions[targetName] = value.get().toPtr<void*>();
        } else {
            out->functions[targetName] = nullptr;
        }
    }

    out->engine = std::move(jitEngine);
    return std::move(out);
}

void SandboxJIT::registerFunctionToDyLib(const std::string &symbol, void *address) {
    _registeredFunctions[symbol] = address;
}
