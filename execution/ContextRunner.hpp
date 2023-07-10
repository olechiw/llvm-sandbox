//
// Created by jolechiw on 7/5/23.
//

#ifndef TESTPROJECT_CONTEXTRUNNER_HPP
#define TESTPROJECT_CONTEXTRUNNER_HPP

#include "../jit/JITCompiler.h"

#include "../model/Diagnostics.h"
#include "../model/FileSystem.h"
#include "context/ExecutionContext.h"
#include "context/FinanceToyContexts.h"

// TODO: RunTimeSwitcher
template<typename Context> requires IsExecutionContext<Context>
class ContextRunner {
public:
    static constexpr auto Name = Context::Name;
    explicit ContextRunner(Diagnostics &diagnostics) : _fileSystem(), _diagnostics(diagnostics) {
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

using HelloWorldRunner = ContextRunner<HelloWorldExecutionContext>;
using FinanceToyRunner = ContextRunner<TestFinanceToy>;


template <typename ...Args>
class RuntimeSwitcher {
public:
    RuntimeSwitcher(auto&& ...constructorArguments) : _currentlyActive{getFirstItemName<Args...>()} {
        addContext<Args...>(std::forward<decltype(constructorArguments)>(constructorArguments)...);
    }

    std::vector<const char *> getNames() {
        std::vector<const char *> out;
        for (const auto &[name, val] : _contexts) {
            out.push_back(name);
        }
        return out;
    }

    void setActive(const char *name) {
        _currentlyActive = name;
    }

    const char *getActive() {
        return _currentlyActive;
    }

    void visit(auto &&callable) {
        std::visit(std::forward<decltype(callable)>(callable), _contexts.find(_currentlyActive)->second);
    }
private:
    std::unordered_map<const char *, std::variant<Args...>> _contexts;
    const char * _currentlyActive;

    template <typename T, typename ...>
    static const char *getFirstItemName() {
        return T::Name;
    }

    template <typename T, typename ...Remaining>
    void addContext(auto && ...constructorArguments) {
        _contexts.insert({T::Name, T(std::forward<decltype(constructorArguments)>(constructorArguments)...)});
        if constexpr (sizeof...(Remaining) > 0) {
            addContext<Remaining...>(std::forward<decltype(constructorArguments)>(constructorArguments)...);
        }
    }
};

using ContextSwitcher = RuntimeSwitcher<HelloWorldRunner, FinanceToyRunner>;


#endif //TESTPROJECT_CONTEXTRUNNER_HPP
