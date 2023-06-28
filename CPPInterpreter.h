//
// Created by jolechiw on 6/27/23.
//

#ifndef TESTPROJECT_CPPINTERPRETER_H
#define TESTPROJECT_CPPINTERPRETER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Support/TargetSelect.h>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/Mangling.h>
#include <llvm/ExecutionEngine/Orc/SymbolStringPool.h>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/CodeGenAction.h>



class CPPInterpreter {
public:
    explicit CPPInterpreter(const std::vector<std::string> &additionalCliArguments = std::vector<std::string>());

    void addFile(const std::string &fileName, const std::string &fileContents);
    void resetFiles();

    struct Context {
        friend class CPPInterpreter;
        std::unordered_map<std::string, void *> functions;
    private:
        Context() = default;
        std::unique_ptr<llvm::orc::LLJIT> engine;
    };

    std::unique_ptr<Context> compile(const std::unordered_set<std::string> &functionsToRetrieve);
private:
    std::vector<std::string> _additionalCliArguments {};
    llvm::IntrusiveRefCntPtr<llvm::vfs::InMemoryFileSystem> _fs {};
    std::vector<std::string> _files {};
};


#endif //TESTPROJECT_CPPINTERPRETER_H
