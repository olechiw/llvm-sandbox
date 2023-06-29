//
// Created by jolechiw on 6/28/23.
//

#include "JITExecutor.h"

JITExecutor::JITExecutor(DiagnosticsConsumer &diagnosticsConsumer) :
    _diagnosticsConsumer(diagnosticsConsumer),
    _interpreter(diagnosticsConsumer) {}

std::unique_ptr<JITExecutor::JITContext> JITExecutor::create(CPPInterpreter::LLVMModuleAndContext llvmModuleAndContext) {
    std::unique_ptr<JITContext> out(new JITContext);

    auto [module, context] = std::move(llvmModuleAndContext);

    std::unordered_map<std::string, std::string> mangledMapping;
    for (const auto &function  : module->getFunctionList()) {
        const auto demangled = llvm::demangle(function.getName().str());
        mangledMapping[demangled] = function.getName().str();
    }

    auto jitEngineExpected = llvm::orc::LLJITBuilder().create();

    if (!jitEngineExpected) {
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::System,
                                   DiagnosticsConsumer::Level::Error,
                                   "Failed To Construct JIT Engine",
                                   toString(jitEngineExpected.takeError())});
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
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::System,
                                   DiagnosticsConsumer::Level::Error,
                                   "Failed to define system dynamic libraries",
                                   toString(std::move(err))});
        return nullptr;
    }
    err = jitEngine->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)));
    if (err) {
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::User,
                                   DiagnosticsConsumer::Level::Error,
                                   "Failed to JIT user's module",
                                   toString(std::move(err))});
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

void JITExecutor::registerFunctionToDyLib(const std::string &symbol, void *address) {
    _registeredFunctions[symbol] = address;
}

std::unique_ptr<JITExecutor::JITContext> JITExecutor::execute(const FileSystem &files, const JITExecutor::DynamicLibraries &dynamicLibraries) {
    // TODO: Compile cpp files one by one?
    _interpreter.resetFiles();
    for (const auto &[fileName, file] : files) {
        _interpreter.addFile(fileName, file.contents, file.metadata.type == File::Type::H);
    }
    for (const auto &[symbol, address] : dynamicLibraries) {
        registerFunctionToDyLib(symbol, address);
    }
    return create(_interpreter.buildModule());
}
