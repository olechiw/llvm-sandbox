//
// Created by jolechiw on 7/7/23.
//

#ifndef TESTPROJECT_INTERPROCESSQUEUE_HPP
#define TESTPROJECT_INTERPROCESSQUEUE_HPP

#include <string>

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

// There is no concept for raw pointers but our types cannot have raw pointers - for simplicity over using an allocator
template<typename T, int SIZE = 65535> requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
class InterprocessQueue {
    static constexpr auto QUEUE_NAME = "shared_ipc_queue";
public:
    using lockfree_ipc_queue = boost::lockfree::spsc_queue<T, boost::lockfree::capacity<SIZE>>;
    InterprocessQueue(const char *name, boost::interprocess::create_only_t) :
        _managedSharedMemory{boost::interprocess::create_only, name, sizeof(lockfree_ipc_queue)},
        _spscQueue {_managedSharedMemory.construct<lockfree_ipc_queue>(QUEUE_NAME)} {
    }
    InterprocessQueue(const char *name, boost::interprocess::open_only_t) :
        _managedSharedMemory{boost::interprocess::open_only, name},
        _spscQueue {_managedSharedMemory.find<lockfree_ipc_queue>(QUEUE_NAME)}{
    }

    const lockfree_ipc_queue &get() const {
        return *_spscQueue;
    }

    lockfree_ipc_queue &get() {
        return *_spscQueue;
    }
private:
    boost::interprocess::managed_shared_memory _managedSharedMemory;
    lockfree_ipc_queue *_spscQueue;
};


#endif //TESTPROJECT_INTERPROCESSQUEUE_HPP