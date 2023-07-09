//
// Created by jolechiw on 7/7/23.
//

#ifndef TESTPROJECT_SUBPROCESS_HPP
#define TESTPROJECT_SUBPROCESS_HPP

#include <memory>
#include <sys/types.h>
#include <csignal>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sys/wait.h>
#include <ext/stdio_filebuf.h>

#include "model/InterprocessQueue.hpp"

// TODO: code review + linter

template <typename T>
class SubProcess {
public:
    template<typename F>
    static std::unique_ptr<SubProcess> spawn(F &subFunctionToRun) {
        // Setup shared memory queue
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        auto sharedMemoryName = boost::uuids::to_string(uuid);
        auto ipcQueue = std::make_unique<InterprocessQueue<T>>(sharedMemoryName.c_str(), boost::interprocess::create_only);

        // Setup stdout/stderr capturing
        int stdOutPipe[2];
        int stdErrPipe[2];
        int status = pipe(stdOutPipe);
        if (status == -1) {
            throw std::runtime_error("Failed to create pipe for stdout capture");
        }
        status = pipe(stdErrPipe);
        if (status == -1) {
            throw std::runtime_error("Failed to create pipe for stderr capture");
        }

        pid_t childProcessPid = fork();
        switch (childProcessPid) {
            case -1:
                throw std::runtime_error("Failed to spawn child process");
            case 0:
                for (auto [pipe, pipeToReplace] : {
                    std::make_tuple(stdOutPipe, STDOUT_FILENO),
                    std::make_tuple(stdErrPipe, STDERR_FILENO)}) {
                    // Close reading end - we only write
                    close(pipe[0]);
                    // setPipeNonBlocking(pipe[1], true);
                    dup2(pipe[1], pipeToReplace);
                }
                subFunctionToRun(ipcQueue->get());
                for (auto pipe : {stdOutPipe, stdErrPipe}) {
                    close(pipe[1]);
                }
                // Exit without cleaning up - this process has barely done anything anyway
                // All cleanup should be done by subFunction
                _Exit(0);
            default:
                for (auto pipe : {stdOutPipe, stdErrPipe}) {
                    // Close writing end - we only read
                    close(pipe[1]);
                    setPipeNonBlocking(pipe[0], false);
                }
                return std::unique_ptr<SubProcess>(new SubProcess{sharedMemoryName, childProcessPid, std::move(ipcQueue), stdOutPipe[0], stdErrPipe[0]});
        }
    }

    bool stop() {
        assert(isRunning());
        kill(_childProcessPid, SIGABRT);
    }

    bool isRunning() {
        _childProcessEnded = _childProcessEnded || waitpid(_childProcessPid, &_childProcessStatus, WNOHANG | WUNTRACED);
        return !_childProcessEnded;
    }

    int getTermSignal() {
        assert(!isRunning());
        if (WIFEXITED(_childProcessStatus)) {
            return 0;
        }
        if (WIFSIGNALED(_childProcessStatus)) {
            return WTERMSIG(_childProcessStatus);
        } else {
            return 0;
        }
    }

    InterprocessQueue<T>::lockfree_ipc_queue &getQueue() {
        return _queue->get();
    }

    int getStdOutFd() {
        return _stdOutFd;
    }

    int getStdErrFd() {
        return _stdErrFd;
    }

    ~SubProcess() {
        close(_stdOutFd);
        close(_stdErrFd);
    }

private:
    static void setPipeNonBlocking(int pipe, bool isChildProcess) {
        int nonBlockStatus = fcntl(pipe, F_SETFL, fcntl(pipe, F_GETFL) | O_NONBLOCK);
        if (nonBlockStatus == -1) {
            if (!isChildProcess) {
                throw std::runtime_error("Failed to set pipe non-blocking");
            } else {
                _Exit(-1);
            }
        }
    }

    SubProcess(const std::string &sharedMemoryName, pid_t childProcessPid, std::unique_ptr<InterprocessQueue<T>> &&queue, int stdOutPipe, int stdErrPipe) :
        _sharedMemoryName{sharedMemoryName}, _sharedMemoryRAII{sharedMemoryName.c_str()},
        _queue{std::move(queue)},
        _childProcessPid{childProcessPid},
        _stdOutFd{stdOutPipe},
        _stdErrFd{stdErrPipe}
        {
        }

    // Shared memory + destroy on destruction of subprocess
    std::string _sharedMemoryName;
    boost::interprocess::remove_shared_memory_on_destroy _sharedMemoryRAII;

    // Message queue!
    std::unique_ptr<InterprocessQueue<T>> _queue;

    pid_t _childProcessPid;
    int _childProcessStatus { 0 };
    bool _childProcessEnded { false };

    // Read pipes for stdout and stderr capturing
    int _stdOutFd, _stdErrFd;

};


#endif //TESTPROJECT_SUBPROCESS_HPP
