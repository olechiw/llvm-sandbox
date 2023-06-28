//
// Created by jolechiw on 6/27/23.
//

#include "CPPInterpreter.h"

CPPInterpreter::CPPInterpreter(const std::vector<std::string> &additionalCliArguments) : _additionalCliArguments(additionalCliArguments) {
    _fs = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
}

std::unique_ptr<CPPInterpreter::Context> CPPInterpreter::compile(const std::unordered_set<std::string> &functionsToRetrieve) {
    std::unique_ptr<Context> out(new Context);

    auto diagOpts = llvm::makeIntrusiveRefCnt<clang::DiagnosticOptions>();
    auto diagIDs = llvm::makeIntrusiveRefCnt<clang::DiagnosticIDs>();
    auto diagPrinter = new clang::TextDiagnosticPrinter(llvm::errs(),
                                                        diagOpts.get());
    auto diagEngine =
            llvm::makeIntrusiveRefCnt<clang::DiagnosticsEngine>(diagIDs,
                                                                diagOpts,
                                                                diagPrinter);

    // Initialize CompilerInvocation
    auto compilerInvocation = std::make_shared<clang::CompilerInvocation>();
    std::vector<const char *> args {};
    args.reserve(_additionalCliArguments.size() + _files.size());
    for (const auto &arg : _additionalCliArguments)
        args.push_back(arg.c_str());
    for (const auto &file : _files)
        args.push_back(file.c_str());
    clang::CompilerInvocation::CreateFromArgs(*compilerInvocation, llvm::ArrayRef(args), *diagEngine);

    clang::CompilerInstance compilerInstance;
    compilerInstance.setInvocation(compilerInvocation);
    compilerInstance.createDiagnostics();

    compilerInstance.setFileManager(new clang::FileManager(clang::FileSystemOptions{}, _fs));
    compilerInstance.createSourceManager(compilerInstance.getFileManager());

    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = LLVM_HOST_TRIPLE;
    auto targetInfo = clang::TargetInfo::CreateTargetInfo(*diagEngine,targetOptions);
    compilerInstance.setTarget(targetInfo);

    auto action = std::make_shared<clang::EmitLLVMOnlyAction>();
    if (!compilerInstance.ExecuteAction(*action)) {
        return out;
    }

    auto context = std::unique_ptr<llvm::LLVMContext>(action->takeLLVMContext());
    auto module = action->takeModule();

    std::unordered_map<std::string, std::string> mangledMapping;
    for (const auto &function  : module->getFunctionList()) {
        const auto demangled = llvm::demangle(function.getName().str());
        for (const auto &targetName : functionsToRetrieve) {
            if (demangled == targetName) {
                mangledMapping[targetName] = function.getName().str();
            }
        }
    }

    auto jitEngineExpected = llvm::orc::LLJITBuilder().create();

    if (!jitEngineExpected) {
        // TODO: Error handling
        return out;
    }
    auto jitEngine = std::move(jitEngineExpected.get());
    if (auto err = jitEngine->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)))) {
        // TODO: Error handling
        return out;
    }

    // std::unique_ptr<llvm::ExecutionEngine> engine(llvm::EngineBuilder(std::move(module)).setEngineKind(llvm::EngineKind::JIT).create());
    for (const auto &targetName : functionsToRetrieve) {
        // Nullptr if not found
        auto value = jitEngine->lookup(mangledMapping[targetName]);
        if (value) {
            out->functions[targetName] = (void*)value.get().toPtr<void*>();
        } else {
            out->functions[targetName] = nullptr;
        }
    }

    out->engine = std::move(jitEngine);
    return std::move(out);
}

void CPPInterpreter::addFile(const std::string &fileName, const std::string &fileContents) {
    auto buffer = llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(fileContents));
     _fs->addFile(llvm::Twine(fileName), 0, std::move(buffer));
    _files.push_back(fileName);
}

void CPPInterpreter::resetFiles() {
    _fs.reset();
    _fs = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
}