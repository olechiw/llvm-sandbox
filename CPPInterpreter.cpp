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
    for (const auto &arg : _additionalCliArguments) args.push_back(arg.c_str());
    for (const auto &file : _files) args.push_back(file.c_str());
    assert(args.size() != 0);
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

    auto module = action->takeModule();

    std::unordered_map<std::string, std::string> mangledMapping;
    for (const auto &function  : module->getFunctionList()) {
        const auto demangled = llvm::demangle(function.getName().str());
        for (const auto &targetName : functionsToRetrieve) {
            std::cout << function.getName().str();
            if (demangled == targetName) {
                std::cout << " " << demangled;
                mangledMapping[targetName] = function.getName().str();
            }
            std::cout << std::endl;
        }
    }

    std::unique_ptr<llvm::ExecutionEngine> engine(llvm::EngineBuilder(std::move(module)).setEngineKind(llvm::EngineKind::JIT).create());

    for (const auto &targetName : functionsToRetrieve) {
        // Nullptr if not found
        out->functions[targetName] = (void*)engine->getFunctionAddress(mangledMapping[targetName]);
    }

    out->engine = std::move(engine);
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