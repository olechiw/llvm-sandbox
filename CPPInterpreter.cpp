//
// Created by jolechiw on 6/27/23.
//

#include "CPPInterpreter.h"

CPPInterpreter::CPPInterpreter(const std::vector<std::string> &additionalCliArguments) : _additionalCliArguments(additionalCliArguments) {
    _fs = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
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

CPPInterpreter::LLVMModuleAndContext CPPInterpreter::buildModule() {
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
        return std::make_tuple(nullptr, nullptr);
    }

    auto context = std::unique_ptr<llvm::LLVMContext>(action->takeLLVMContext());
    auto module = action->takeModule();
    return std::make_tuple(std::move(module), std::move(context));
}