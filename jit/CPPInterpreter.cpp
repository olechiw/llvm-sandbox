//
// Created by jolechiw on 6/27/23.
//

#include "CPPInterpreter.h"

CPPInterpreter::CPPInterpreter(Diagnostics &diagnostics, const std::vector<std::string> &additionalCliArguments) : _diagnostics(diagnostics), _additionalCliArguments(additionalCliArguments) {
    _memoryFileSystem = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
    auto physicalWorkingDir = llvm::vfs::getRealFileSystem()->getCurrentWorkingDirectory();
    if (physicalWorkingDir) {
        _memoryFileSystem->setCurrentWorkingDirectory(physicalWorkingDir.get());
    }
}

void CPPInterpreter::addFile(const std::string &fileName, const std::string &fileContents, bool header) {
    auto buffer = llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(fileContents));
     _memoryFileSystem->addFile(llvm::Twine(fileName), 0, std::move(buffer));
     if (!header)
        _files.push_back(fileName);
}

void CPPInterpreter::resetFiles() {
    _memoryFileSystem = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
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
    args.push_back("-stdlib=libstdc++");
    clang::CompilerInvocation::CreateFromArgs(*compilerInvocation, llvm::ArrayRef(args), *diagEngine);
    compilerInvocation->getLangOpts()->CPlusPlus = 1;
    compilerInvocation->getLangOpts()->CPlusPlus20 = 1;

    clang::CompilerInstance compilerInstance;
    compilerInstance.setInvocation(compilerInvocation);
    compilerInstance.setDiagnostics(diagEngine);

    auto combinedFileSystem = llvm::makeIntrusiveRefCnt<llvm::vfs::OverlayFileSystem>(llvm::vfs::getRealFileSystem());
    combinedFileSystem->pushOverlay(_memoryFileSystem);
    compilerInstance.setFileManager(new clang::FileManager(clang::FileSystemOptions{}, combinedFileSystem));
    compilerInstance.createSourceManager(compilerInstance.getFileManager());

    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = LLVM_HOST_TRIPLE;
    auto targetInfo = clang::TargetInfo::CreateTargetInfo(*diagEngine,targetOptions);
    compilerInstance.setTarget(targetInfo);
    // clang++-16 -### hello.cpp
    // yields -internal-isystem and internal-externc-isystem:
    auto iSystem = {"/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11" , "/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../include/x86_64-linux-gnu/c++/11" , "/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/backward" , "/usr/lib/llvm-16/lib/clang/16/include" , "/usr/local/include" , "/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../x86_64-linux-gnu/include"};
    auto iCExtern = {"/usr/include/x86_64-linux-gnu", "/include", "/usr/include"};
    for (const auto &path : iSystem) {
        compilerInstance.getHeaderSearchOpts().AddPath(path,
                                                       clang::frontend::IncludeDirGroup::System,
                                                       false,
                                                       false);
    }
    for (const auto &path : iCExtern) {
        compilerInstance.getHeaderSearchOpts().AddPath(path,
                                                       clang::frontend::IncludeDirGroup::ExternCSystem,
                                                       false,
                                                       false);
    }

    auto action = std::make_shared<clang::EmitLLVMOnlyAction>();
    if (!compilerInstance.ExecuteAction(*action)) {
        errorOutput = errorStream.str();
        _diagnostics.push({Diagnostics::Type::User,
                                   Diagnostics::Level::Error,
                                   "Failed to Compile",
                                   errorOutput});
        return std::make_tuple(nullptr, nullptr);
    }

    auto context = std::unique_ptr<llvm::LLVMContext>(action->takeLLVMContext());
    auto module = action->takeModule();
    return std::make_tuple(std::move(module), std::move(context));
}