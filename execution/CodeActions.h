//
// Created by jolechiw on 7/5/23.
//

#ifndef TESTPROJECT_CODEACTIONS_H
#define TESTPROJECT_CODEACTIONS_H

#include "../jit/JITCompiler.h"

#include "../model/Diagnostics.h"
#include "../model/FileSystem.h"
#include "context/ExecutionContext.h"
#include "context/FinanceToyContexts.h"

// TODO: RunTimeSwitcher
template<typename Context> requires IsExecutionContext<Context>
class CodeActions {
public:
    static constexpr auto Name = Context::Name;
    explicit CodeActions(Diagnostics &diagnostics) : _fileSystem(), _diagnostics(diagnostics) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();

        for (const auto &[fileName, file]: Context::getInstance().StarterFiles) {
            _fileSystem.createOrOverwriteFile(file);
        }
        for (const auto &[fileName, file]: Context::getInstance().HelperFiles) {
            _fileSystem.createOrOverwriteFile(file);
        }
    }

    void build() {
        _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Info, "Compiling..."});
        _compiledCode = JITCompiler::compile(_diagnostics, _fileSystem, Context::getInstance().DynamicLibraries);
        const auto result = _compiledCode ? "Compiled" : "Failed to Compile";
        _diagnostics.push({Diagnostics::Type::System, Diagnostics::Level::Info, result});
    }

    void runBuiltCode() {
        if (_compiledCode) {
            Context::getInstance().run(*_compiledCode);
        }
    }

    bool isRunning() {
        return Context::getInstance().isRunning();
    }

    void render() {
        Context::getInstance().render();
    }

    std::vector<std::string> takeOutput() {
        return Context::getInstance().takeOutput();
    }

    const FileSystem &getFileSystem() const {
        return _fileSystem;
    }

    FileSystem &getFileSystem() {
        return _fileSystem;
    }

private:
    Diagnostics &_diagnostics;
    FileSystem _fileSystem;
    std::unique_ptr<JITCompiler::CompiledCode> _compiledCode{nullptr};
};

using HelloWorldCodeActions = CodeActions<HelloWorldExecutionContext>;
using FinanceToyActions = CodeActions<TestFinanceToy>;


template <typename ...Args>
class RunTimeSwitcher {
public:
    RunTimeSwitcher(auto&& ...constructorArguments) {
        addSwitch<Args...>(std::forward<decltype(constructorArguments)>(constructorArguments)...);
        setFirstAsActive<Args...>();
    }

    std::vector<std::string> getNames() {
        std::vector<std::string> out;
        for (const auto &[name, val] : _codeActions) {
            out.push_back(name);
        }
        return out;
    }

    void setActive(const std::string &name) {
        _currentlyActive = name;
    }

    void visit(auto &&callable) {
        std::visit(std::forward<decltype(callable)>(callable), _codeActions.find(_currentlyActive)->second);
    }
private:
    std::unordered_map<std::string, std::variant<Args...>> _codeActions;
    std::string _currentlyActive;

    template <typename T, typename ...>
    void setFirstAsActive() {
        _currentlyActive = T::Name;
    }

    template <typename T, typename ...Remaining>
    void addSwitch(auto && ...constructorArguments) {
        _codeActions.insert({T::Name, T(std::forward<decltype(constructorArguments)>(constructorArguments)...)});
        if constexpr (sizeof...(Remaining) > 0) {
            addSwitch<Remaining...>(std::forward<decltype(constructorArguments)>(constructorArguments)...);
        }
    }
};

using CodeActionsSwitcher = RunTimeSwitcher<HelloWorldCodeActions, FinanceToyActions>;


#endif //TESTPROJECT_CODEACTIONS_H
