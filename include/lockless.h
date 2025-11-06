#ifndef LOCKLESS_H
#define LOCKLESS_H

#include <windows.h>
#include <atomic>
#include <iostream>

template<typename T>
class SharedMemory {
  public:
    std::atomic<T> head;
    std::atomic<T> tail;

    HANDLE CreateSharedMemory(std::string& name, size_t& size);
    HANDLE OpenSharedMemory(std::string& name);
    void CleanupSharedMemory(std::string& name);
};

#include "../src/lockless.cpp"

#endif