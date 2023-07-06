//
// Created by jolechiw on 6/27/23.
//

#ifndef TESTPROJECT_CPPINTERPRETER_H
#define TESTPROJECT_CPPINTERPRETER_H

#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <sstream>

#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Support/TargetSelect.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Lex/HeaderSearchOptions.h>
#include <clang/Lex/PreprocessorOptions.h>
#include "../model/Diagnostics.h"


class CPPInterpreter {
public:
    using LLVMModuleAndContext = std::tuple<std::unique_ptr<llvm::Module>, std::unique_ptr<llvm::LLVMContext>>;

    explicit CPPInterpreter(Diagnostics &diagnostics, const std::vector<std::string> &additionalCliArguments = std::vector<std::string>());

    void addFile(const std::string &fileName, const std::string &fileContents, bool header = false);
    void resetFiles();

    LLVMModuleAndContext buildModule();
private:
    std::vector<std::string> _additionalCliArguments {};
    llvm::IntrusiveRefCntPtr<llvm::vfs::InMemoryFileSystem> _memoryFileSystem {};
    std::vector<std::string> _files {};
    Diagnostics &_diagnostics;
};


#endif //TESTPROJECT_CPPINTERPRETER_H
