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

// TODO: code review + linter

class SubProcess {
public:
    template<typename F, typename ...Args>
    static std::unique_ptr<SubProcess> spawn(F &&subFunctionToRun, Args &&...args);

    void stop() {
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

    [[nodiscard]] int getStdOutFd() const {
        return _stdOutFd;
    }

    [[nodiscard]] int getStdErrFd() const {
        return _stdErrFd;
    }

    ~SubProcess() {
        close(_stdOutFd);
        close(_stdErrFd);
    }

private:
    SubProcess(pid_t childProcessPid, int stdOutPipe, int stdErrPipe)
            : _childProcessPid{childProcessPid}, _stdOutFd{stdOutPipe}, _stdErrFd{stdErrPipe} {
    }

    pid_t _childProcessPid;
    int _childProcessStatus{0};
    bool _childProcessEnded{false};

    // Read pipes for stdout and stderr capturing
    int _stdOutFd, _stdErrFd;

    static void setPipeNonBlocking(int pipe) {
        int nonBlockStatus = fcntl(pipe, F_SETFL, fcntl(pipe, F_GETFL) | O_NONBLOCK);
        if (nonBlockStatus == -1) {
            // TODO: warn?
            throw std::runtime_error("Failed to set pipe non-blocking");
        }
    }
};

template<typename F, typename ...Args>
std::unique_ptr<SubProcess> SubProcess::spawn(F &&subFunctionToRun, Args &&...args) {
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
            // Child
        case 0:
            // Replace stdout and stderr
            close(stdOutPipe[0]);
            dup2(stdOutPipe[1], STDOUT_FILENO);
            close(stdErrPipe[0]);
            dup2(stdErrPipe[1], STDERR_FILENO);

            subFunctionToRun(std::forward<Args>(args)...);

            close(stdOutPipe[1]);
            close(stdErrPipe[1]);

            // Exit without cleaning up - this process has barely done anything anyway
            // All cleanup should be done by subFunction
//            _Exit(0);
            exit(0);
            // Parent
        default:
            close(stdOutPipe[1]);
            setPipeNonBlocking(stdOutPipe[0]);
            close(stdErrPipe[1]);
            setPipeNonBlocking(stdErrPipe[0]);

            return std::unique_ptr<SubProcess>(
                    new SubProcess{childProcessPid, stdOutPipe[0], stdErrPipe[0]});
    }
}


#endif //TESTPROJECT_SUBPROCESS_HPP
