#ifndef LOCKLESS_H
#define LOCKLESS_H

#include <windows.h>
#include <atomic>
#include <iostream>

template<typename T>
struct CircularBufferHeader {
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    size_t capacity;
    size_t elementSize;
};

template<typename T>
class SharedMemory {
  public:
    CircularBufferHeader<T>* header;
    T* data;
    HANDLE hMapFile;
    void* pBuf;

    HANDLE CreateSharedMemory(std::string& name, size_t& size);
    HANDLE OpenSharedMemory(std::string& name);
    void CleanupSharedMemory();
    void EnqueueData(const T& item);
    bool ReadData(T& item);
};

#include "../src/lockless.cpp"

#endif