//
// Created by jolechiw on 7/7/23.
//

#ifndef TESTPROJECT_INTERPROCESSQUEUE_HPP
#define TESTPROJECT_INTERPROCESSQUEUE_HPP

#include <string>

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

// There is no concept for raw pointers but our types cannot have raw pointers - for simplicity over using an allocator
template<typename T>
class SharedMemoryContainer {
public:
    using ContainerType = T;
    // This class does not actually own anything so all default operators are fine
    SharedMemoryContainer(const char *name, boost::interprocess::create_only_t) :
        _sharedMemoryObject{boost::interprocess::create_only, name, boost::interprocess::read_write}
    {
        _sharedMemoryObject.truncate(sizeof(ContainerType));
        _mappedMemoryRegion = {_sharedMemoryObject, boost::interprocess::read_write};
        _container = new(_mappedMemoryRegion.get_address()) ContainerType();
    }

    SharedMemoryContainer(const char *name, boost::interprocess::open_only_t) :
        _sharedMemoryObject{boost::interprocess::open_only, name, boost::interprocess::read_write},
        _mappedMemoryRegion{_sharedMemoryObject, boost::interprocess::read_write}
    {
        _container = reinterpret_cast<ContainerType*>(_mappedMemoryRegion.get_address());
    }

    const ContainerType &get() const {
        return *_container;
    }

    ContainerType &get() {
        return *_container;
    }
private:
    boost::interprocess::shared_memory_object _sharedMemoryObject;
    boost::interprocess::mapped_region _mappedMemoryRegion;
    ContainerType *_container;
};

// Data type has to be standard layout - should also not have pointers etc.
template<typename DataType, int SIZE = 1024> requires std::is_standard_layout_v<DataType>
using InterprocessQueue = SharedMemoryContainer<boost::lockfree::spsc_queue<DataType, boost::lockfree::capacity<SIZE>>>;


#endif //TESTPROJECT_INTERPROCESSQUEUE_HPP
