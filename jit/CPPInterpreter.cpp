//
// Created by jolechiw on 6/27/23.
//

#include "CPPInterpreter.h"

CPPInterpreter::CPPInterpreter(DiagnosticsConsumer &diagnosticsConsumer, const std::vector<std::string> &additionalCliArguments) : _diagnosticsConsumer(diagnosticsConsumer), _additionalCliArguments(additionalCliArguments) {
    _fs = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
}

void CPPInterpreter::addFile(const std::string &fileName, const std::string &fileContents, bool header) {
    auto buffer = llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(fileContents));
     _fs->addFile(llvm::Twine(fileName), 0, std::move(buffer));
     if (!header)
        _files.push_back(fileName);
}

void CPPInterpreter::resetFiles() {
    _fs = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
}

CPPInterpreter::LLVMModuleAndContext CPPInterpreter::buildModule() {
    auto diagOpts = llvm::makeIntrusiveRefCnt<clang::DiagnosticOptions>();
    auto diagIDs = llvm::makeIntrusiveRefCnt<clang::DiagnosticIDs>();
    std::string errorOutput;
    llvm::raw_string_ostream errorStream(errorOutput);
    auto diagPrinter = new clang::TextDiagnosticPrinter(errorStream,
                                                        diagOpts.get());
    auto diagEngine = new clang::DiagnosticsEngine(diagIDs,
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
    compilerInvocation->getLangOpts()->CPlusPlus = 1;
    compilerInvocation->getLangOpts()->CPlusPlus20 = 1;

    clang::CompilerInstance compilerInstance;
    compilerInstance.setInvocation(compilerInvocation);
    compilerInstance.setDiagnostics(diagEngine);

    compilerInstance.setFileManager(new clang::FileManager(clang::FileSystemOptions{}, _fs));
    compilerInstance.createSourceManager(compilerInstance.getFileManager());

    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = LLVM_HOST_TRIPLE;
    auto targetInfo = clang::TargetInfo::CreateTargetInfo(*diagEngine,targetOptions);
    compilerInstance.setTarget(targetInfo);

    auto action = std::make_shared<clang::EmitLLVMOnlyAction>();
    if (!compilerInstance.ExecuteAction(*action)) {
        errorOutput = errorStream.str();
        _diagnosticsConsumer.push({DiagnosticsConsumer::Type::User,
                                   DiagnosticsConsumer::Level::Error,
                                   "Failed to Compile",
                                   errorOutput});
        return std::make_tuple(nullptr, nullptr);
    }

    auto context = std::unique_ptr<llvm::LLVMContext>(action->takeLLVMContext());
    auto module = action->takeModule();
    return std::make_tuple(std::move(module), std::move(context));
}