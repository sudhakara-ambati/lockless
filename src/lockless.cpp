#include "../include/lockless.h"
#include <windows.h>
#include <atomic>
#include <iostream>

template<typename T>
HANDLE SharedMemory<T>::CreateSharedMemory(std::string& name, size_t& size) {

    HANDLE sharedMemorySpace = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        static_cast<DWORD>(size),
        name.c_str());

    if (sharedMemorySpace == nullptr) {
        std::cerr << "Could not create file mapping!\n";
        return nullptr;
    }

    pBuf = MapViewOfFile(sharedMemorySpace, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (pBuf == nullptr) {
        std::cerr << "Could not map view of file!\n";
        CloseHandle(sharedMemorySpace);
        return nullptr;
    }

    header = reinterpret_cast<CircularBufferHeader<T>*>(pBuf);
    data = reinterpret_cast<T*>(reinterpret_cast<char*>(pBuf) + sizeof(CircularBufferHeader<T>));
    hMapFile = sharedMemorySpace;

    header->head.store(0, std::memory_order_release);
    header->tail.store(0, std::memory_order_release);
    header->capacity = (size - sizeof(CircularBufferHeader<T>)) / sizeof(T);
    header->elementSize = sizeof(T);

    if (header->capacity == 0) {
        std::cerr << "Shared memory too small for any elements.\n";
        UnmapViewOfFile(pBuf);
        CloseHandle(sharedMemorySpace);
        pBuf = nullptr;
        header = nullptr;
        data = nullptr;
        hMapFile = nullptr;
        return nullptr;
    }

    std::cout << "Shared memory created: " << name << " of size " << size << "\n";
    return sharedMemorySpace;
}

template<typename T>
HANDLE SharedMemory<T>::OpenSharedMemory(std::string& name) {

    HANDLE sharedMemorySpace = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        name.c_str());

    if (sharedMemorySpace == nullptr) {
        std::cerr << "Failed to open shared memory: " << name << "\n";
        return nullptr;
    }

    pBuf = MapViewOfFile(sharedMemorySpace, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (pBuf == nullptr) {
        std::cerr << "Failed to map view for: " << name << "\n";
        CloseHandle(sharedMemorySpace);
        return nullptr;
    }

    header = reinterpret_cast<CircularBufferHeader<T>*>(pBuf);
    data = reinterpret_cast<T*>(reinterpret_cast<char*>(pBuf) + sizeof(CircularBufferHeader<T>));
    hMapFile = sharedMemorySpace;

    if (header->elementSize != sizeof(T)) {
        std::cerr << "Element size mismatch in shared memory.\n";
        UnmapViewOfFile(pBuf);
        CloseHandle(sharedMemorySpace);
        pBuf = nullptr;
        header = nullptr;
        data = nullptr;
        hMapFile = nullptr;
        return nullptr;
    }

    return sharedMemorySpace;
}

template<typename T>
void SharedMemory<T>::CleanupSharedMemory() {
    if (pBuf != nullptr) {
        UnmapViewOfFile(pBuf);
        pBuf = nullptr;
    }

    if (hMapFile != nullptr) {
        CloseHandle(hMapFile);
        hMapFile = nullptr;
    }

    header = nullptr;
    data = nullptr;
}

template<typename T>
void SharedMemory<T>::EnqueueData(const T& item) {
    if (header == nullptr || data == nullptr) {
        std::cerr << "Shared memory not initialized for enqueue.\n";
        return;
    }

    if (header->elementSize != sizeof(T)) {
        std::cerr << "Element size mismatch in enqueue.\n";
        return;
    }

    const size_t capacity = header->capacity;
    if (capacity == 0) {
        std::cerr << "Buffer capacity is zero.\n";
        return;
    }

    const size_t head = header->head.load(std::memory_order_acquire);
    const size_t tail = header->tail.load(std::memory_order_acquire);
    const size_t nextHead = (head + 1) % capacity;

    if (nextHead == tail) {
        std::cerr << "Buffer is full.\n";
        return;
    }

    data[head] = item;
    header->head.store(nextHead, std::memory_order_release);
}

template<typename T>
bool SharedMemory<T>::ReadData(T& item) {
    if (header == nullptr || data == nullptr) {
        std::cerr << "Shared memory not initialized for read.\n";
        return false;
    }

    const size_t capacity = header->capacity;
    if (capacity == 0) {
        std::cerr << "Buffer capacity is zero.\n";
        return false;
    }

    const size_t head = header->head.load(std::memory_order_acquire);
    const size_t tail = header->tail.load(std::memory_order_acquire);

    if (head == tail) {
        return false;
    }

    item = data[tail];
    const size_t nextTail = (tail + 1) % capacity;
    header->tail.store(nextTail, std::memory_order_release);
    
    return true;
}

/*
Plan:
- create circular buffer in memory with head and tail
- read from head, write to tail
- atomicity and smooth cleanup 
*/