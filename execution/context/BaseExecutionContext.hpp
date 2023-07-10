//
// Created by jolechiw on 7/5/23.
//

#ifndef TESTPROJECT_BASEEXECUTIONCONTEXT_HPP
#define TESTPROJECT_BASEEXECUTIONCONTEXT_HPP

#include <string>
#include <unordered_map>
#include <iostream>
#include <cassert>

#include "../../model/FileSystem.h"
#include "../../jit/JITCompiler.h"
#include "../../model/InterprocessQueue.hpp"
#include "../SubProcess.hpp"

template<typename T, typename M> concept IsRunnableExecutionContextWithQueue = requires {
    std::is_invocable_v<decltype(&T::runImpl), T &, JITCompiler::CompiledCode &, typename InterprocessQueue<M>::ContainerType &>;
};

template<typename T> concept IsRunnableExecutionContext = requires {
    std::is_invocable_v<decltype(&T::runImpl), T &, JITCompiler::CompiledCode &>;
};


template<typename M>
struct SharedMemory {
    SharedMemory(const std::string &sharedMemoryName) :
        sharedMemoryName{sharedMemoryName},
        sharedMemoryRAII{sharedMemoryName.c_str()},
        sharedMemoryQueue{sharedMemoryName.c_str(), boost::interprocess::create_only} {

    }
    std::string sharedMemoryName;
    boost::interprocess::remove_shared_memory_on_destroy sharedMemoryRAII;
    InterprocessQueue<M> sharedMemoryQueue;
};

template<>
struct SharedMemory<void> {
};

template<typename T, typename M = void>
class BaseExecutionContext {
public:
    using MessageType = M;

    static T &getInstance() {
        return _instance;
    }

    void run(JITCompiler::CompiledCode &compiledCode) {
        assert(!isRunning());
        _isChildProcess = true;
        try {
            if constexpr (IsRunnableExecutionContextWithQueue<T, MessageType>) {
                runWithQueue(compiledCode);
            } else {
                static_assert(IsRunnableExecutionContext<T>);
                runWithoutQueue(compiledCode);
            }
        } catch (std::runtime_error e) {
            output.push_back(e.what() + std::string("\n"));
        }
        _isChildProcess = false;
    }

    void render() {
        if (!_subProcess) return;
        bool isRunning = _subProcess->isRunning();

        auto captureOutput = [&](int fd) {
            char buffer[2048];
            ssize_t result = read(fd, buffer, sizeof(buffer));
            while (result > 0) {
                output.emplace_back(std::string_view(buffer, result));
                result = read(fd, buffer, sizeof(buffer));
            }
        };
        captureOutput(_subProcess->getStdOutFd());
        captureOutput(_subProcess->getStdErrFd());

        if constexpr (IsRunnableExecutionContextWithQueue<T, MessageType>) {
            assert(_sharedMemory);
            static_cast<T *>(this)->renderImpl(_sharedMemory->sharedMemoryQueue.get());
        } else {
            static_cast<T *>(this)->renderImpl();
        }

        if (!isRunning) {
            switch (_subProcess->getTermSignal()) {
                case SIGSEGV:
                    output.emplace_back("Terminated with SIGSEGV\n");
                    break;
                case SIGABRT:
                    output.emplace_back("Terminated with SIGABRT\n");
                    break;
                case SIGKILL:
                    output.emplace_back("Terminated with SIGKILL\n");
                    break;
                default:
                    output.emplace_back("Exited normally\n");
                    break;
            }
            _subProcess = nullptr;
            _sharedMemory = nullptr;
        }
    }

    void pushOutput(const std::string &value) {
        assert(!_isChildProcess);
        output.push_back(value);
    }

    bool isRunning() {
        return _subProcess && _subProcess->isRunning();
    }

    std::vector<std::string> takeOutput() {
        return std::move(output);
    }

protected:
    BaseExecutionContext() = default;

private:
    void runWithQueue(JITCompiler::CompiledCode &compiledCode) {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        auto sharedMemoryName = boost::uuids::to_string(uuid);
        _sharedMemory = std::unique_ptr<SharedMemory<M>>(new SharedMemory<M>{sharedMemoryName});
        T &impl = *static_cast<T *>(this);
        auto wrapper = [&](JITCompiler::CompiledCode &compiledCode, InterprocessQueue<M>::ContainerType &queue) {
            impl.runImpl(compiledCode, queue);
        };
        _subProcess = SubProcess::spawn(wrapper, compiledCode, _sharedMemory->sharedMemoryQueue.get());
    }

    void runWithoutQueue(JITCompiler::CompiledCode &compiledCode) {
        T &impl = *static_cast<T *>(this);
        auto wrapper = [&](JITCompiler::CompiledCode &compiledCode) {
            impl.runImpl(compiledCode);
        };
        _subProcess = SubProcess::spawn(wrapper, compiledCode);
    }

    static T _instance;
    std::vector<std::string> output{};
    std::unique_ptr<SubProcess> _subProcess{nullptr};
    std::unique_ptr<SharedMemory<M>> _sharedMemory{nullptr};
    bool _isChildProcess{false};
};

template<typename T, typename M> T BaseExecutionContext<T, M>::_instance;

class HelloWorldExecutionContext : public BaseExecutionContext<HelloWorldExecutionContext> {
public:
    // Required name of context
    static constexpr auto Name = "Hello World";

    // Optional inclusion of static libraries
    static void print(const char *value) {
        std::cout << value;
    }

    // Required context info including starting filesystems (can all be empty)
    const Files StarterFiles{{"main.cpp", {{"main.cpp", File::Type::CPP, false},
                                           "#include \"system_headers.h\"\n\nint main() {\n\tprint(\"Hello World\\n\");\n}"}}};
    const Files HelperFiles{
            {"system_headers.h", {{"system_headers.h", File::Type::H, true}, "extern void print(const char*);"}}};
    const JITCompiler::DynamicLibraries DynamicLibraries{
            {"print(char const*)", (void *) HelloWorldExecutionContext::print}};
private:
    friend class BaseExecutionContext<HelloWorldExecutionContext>;

    // Always runs in the child process
    void runImpl(JITCompiler::CompiledCode &compiledCode) {
        using MainType = int (*)();
        auto function = compiledCode.functions.find("main");
        if (function != compiledCode.functions.end()) {
            ((MainType) function->second)();
        }
    }

    // Always runs in the parent process
    void renderImpl() {
    }
};

template<typename T> concept IsExecutionContext = requires {
    std::is_base_of_v<BaseExecutionContext<T>, T> && std::is_same_v<decltype(T::Name), const char *const> &&
    std::is_same_v<decltype(T::StarterFiles), const Files> && std::is_same_v<decltype(T::HelperFiles), const Files> &&
    std::is_same_v<decltype(T::DynamicLibraries), const JITCompiler::DynamicLibraries>;
};


#endif //TESTPROJECT_BASEEXECUTIONCONTEXT_HPP
