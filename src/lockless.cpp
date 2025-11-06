#include "../include/lockless.h"
#include <windows.h>
#include <atomic>
#include <iostream>

template<typename T>
HANDLE SharedMemory<T>::CreateSharedMemory(std::string& name, size_t& size) {
    HANDLE sharedMemorySpace;
    sharedMemorySpace = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        static_cast<DWORD>(size),
        name.c_str());
        
    std::cout << "Shared memory created: " << name << " of size " << size << std::endl;
    return sharedMemorySpace;
}

template<typename T>
HANDLE SharedMemory<T>::OpenSharedMemory(std::string& name) {
    HANDLE sharedMemorySpace;
    sharedMemorySpace = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        name.c_str());

    if (sharedMemorySpace != NULL) {
        return sharedMemorySpace;
    }
    else {
        std::cerr << "Failed to open shared memory: " << name << std::endl;
        return NULL;
    }
}

template<typename T>
void SharedMemory<T>::CleanupSharedMemory(std::string& name) {
    HANDLE sharedMemorySpace = OpenSharedMemory(name);

    if (sharedMemorySpace != NULL) {
        CloseHandle(sharedMemorySpace);
        std::cout << "Shared memory cleaned up: " << name << std::endl;
    }
    else {
        std::cerr << "Failed to clean up shared memory: " << name << std::endl;
    }
}

/*
Plan:
- create circular buffer in memory with head and tail
- read from head, write to tail
- atomicity and smooth cleanup 
*/