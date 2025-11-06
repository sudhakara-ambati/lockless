#include <windows.h>
#include <atomic>
#include <iostream>

template<typename T>
class SharedMemory {
  public:
    std::atomic<T> head;
    std::atomic<T> tail;

    void CreateSharedMemory(std::string name, size_t size) {
        // implementation for creating shared memory
    }

    void OpenSharedMemory(std::string name) {
        // implementation for opening shared memory
    }

};

/*
Plan:
- create circular buffer in memory with head and tail
- read from head, write to tail
- atomicity and smooth cleanup 
*/