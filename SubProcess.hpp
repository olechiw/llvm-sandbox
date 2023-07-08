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

#include "model/InterprocessQueue.hpp"

// TODO: code review + linter

template <typename T>
class SubProcess {
public:
    template<typename F>
    static std::shared_ptr<SubProcess> spawn(F &subFunctionToRun) {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        auto sharedMemoryName = boost::uuids::to_string(uuid);
        auto ipcQueue = std::make_shared<InterprocessQueue<T>>(sharedMemoryName.c_str(), boost::interprocess::create_only);
        pid_t childProcessPid = fork();
        if (childProcessPid == 0){
            subFunctionToRun(ipcQueue->get());
            // Exit without cleaning up - this process has barely done anything anyway
            // All cleanup should be done by subFunction
            _Exit(0);
        }
//        std::this_thread::sleep_for(std::chrono::seconds(1));
        return std::shared_ptr<SubProcess>(new SubProcess{sharedMemoryName, childProcessPid, ipcQueue});
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


private:
    SubProcess(std::string sharedMemoryName, pid_t childProcessPid, std::shared_ptr<InterprocessQueue<T>> &queue) :
        _sharedMemoryName{sharedMemoryName}, _sharedMemoryRAII{sharedMemoryName.c_str()}, _queue{queue}, _childProcessPid{childProcessPid} {}
    std::string _sharedMemoryName;
    boost::interprocess::remove_shared_memory_on_destroy _sharedMemoryRAII;
    std::shared_ptr<InterprocessQueue<T>> _queue;
    pid_t _childProcessPid;
    int _childProcessStatus { 0 };
    bool _childProcessEnded { false };
};


#endif //TESTPROJECT_SUBPROCESS_HPP
