//
// Created by jolechiw on 6/28/23.
//

#include "JITCompiler.h"

std::unique_ptr<JITCompiler::CompiledCode>
JITCompiler::create(Diagnostics &diagnostics, CPPInterpreter::LLVMModuleAndContext llvmModuleAndContext,
                    const JITCompiler::DynamicLibraries &dynamicLibraries) {
    // CompiledCode has private constructor
    std::unique_ptr<CompiledCode> out(new CompiledCode);

    auto[module, context] = std::move(llvmModuleAndContext);

    auto jitEngineExpected = llvm::orc::LLJITBuilder().create();

    if (!jitEngineExpected) {
        diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Error, "Failed To Construct JIT Engine",
                          toString(jitEngineExpected.takeError())});
        return nullptr;
    }
    auto jitEngine = std::move(jitEngineExpected.get());

    // Create dynamic library generator from system process
    auto &dyLib = jitEngine->getMainJITDylib();
    auto library = llvm::sys::DynamicLibrary::getPermanentLibrary(nullptr);
    auto prefix = jitEngine->getDataLayout().getGlobalPrefix();
    dyLib.addGenerator(std::make_unique<llvm::orc::DynamicLibrarySearchGenerator>(library, prefix));

    // Register symbols from DynamicLibraries
    auto symbolPool = jitEngine->getExecutionSession().getSymbolStringPool();
    llvm::DenseMap<llvm::orc::SymbolStringPtr, llvm::JITEvaluatedSymbol> symbolsToRegister;
    // Get un-mangled to mangled name mapping of symbols in user code
    std::unordered_map<std::string, std::string> mangledMapping;
    for (const auto &function: module->getFunctionList()) {
        const auto demangled = llvm::demangle(function.getName().str());
        mangledMapping[demangled] = function.getName().str();
    }
    for (const auto &[symbolName, addr]: dynamicLibraries) {
        // Only intern the symbols that are used
        if (mangledMapping.find(symbolName) == mangledMapping.end())
            continue;
        symbolsToRegister.insert(
                {symbolPool->intern(mangledMapping[symbolName]), llvm::JITEvaluatedSymbol::fromPointer(addr)});
    }
    auto err = dyLib.define(llvm::orc::absoluteSymbols(symbolsToRegister));
    if (err) {
        diagnostics.push(
                {Diagnostics::Type::System, Diagnostics::Level::Error, "Failed to define system dynamic libraries",
                 toString(std::move(err))});
        return nullptr;
    }

    // Add module to engine
    err = jitEngine->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)));
    if (err) {
        diagnostics.push({Diagnostics::Type::User, Diagnostics::Level::Error, "Failed to JIT user's module",
                          toString(std::move(err))});
        return nullptr;
    }

    // Index functions into output object after compilation
    for (const auto &[targetName, mangledName]: mangledMapping) {
        // We are providing the mangled name because it was mangled by the frontend (clang)
        auto value = jitEngine->lookup(mangledName);
        if (value) {
            out->functions[targetName] = value.get().toPtr<void *>();
        } else {
            out->functions[targetName] = nullptr;
        }
    }

    // TODO: more diagnostics for errors (see cling)

    out->engine = std::move(jitEngine);
    return std::move(out);
}

std::unique_ptr<JITCompiler::CompiledCode>
JITCompiler::compile(Diagnostics &diagnostics, const FileSystem &files, const DynamicLibraries &dynamicLibraries) {
    // TODO: Compile cpp files one by one?
    CPPInterpreter interpreter(diagnostics);
    for (const auto &[fileName, file]: files.getFiles()) {
        interpreter.addFile(fileName, file.contents, file.metadata.type == File::Type::H);
    }
    auto[context, module] = interpreter.buildModule();
    if (!context || !module) {
        return nullptr;
    }
    return create(diagnostics, std::make_tuple(std::move(context), std::move(module)), dynamicLibraries);
}
