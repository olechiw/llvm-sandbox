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

#include "model/InterprocessQueue.hpp"

template <typename T>
class SubProcess {
public:
    template<typename F>
    static std::shared_ptr<SubProcess> spawn(F subFunctionToRun) {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        auto sharedMemoryName = boost::uuids::to_string(uuid);
        InterprocessQueue<T> ipcQueue(sharedMemoryName.c_str(), boost::interprocess::create_only);
        pid_t childProcessPid = fork();
        if (!childProcessPid){
            F(ipcQueue.get());
            exit(0);
        }
        return std::make_shared(ipcQueue,sharedMemoryName, {sharedMemoryName.c_str()}, childProcessPid);
    }

    bool empty() {
        return _queue.empty();
    }

    T &front() {
        return _queue.front();
    }

    T take() {
        T out = front();
        _queue.pop();
        return out;
    }

    bool stop() {
        kill(_childProcessPid, SIGABRT);
    }
private:
    SubProcess() = default;
    InterprocessQueue<T>::lockfree_ipc_queue &_queue;
    std::string _sharedMemoryName;
    boost::interprocess::remove_shared_memory_on_destroy _sharedMemoryRAII;
    pid_t _childProcessPid;
};


#endif //TESTPROJECT_SUBPROCESS_HPP
