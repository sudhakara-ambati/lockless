#include "../include/lockless.h"
#include <string>
#include <thread>
#include <chrono>

int main() {
    SharedMemory<int> sharedMem;
    
    sharedMem.head.store(0);
    sharedMem.tail.store(0);
    
    std::string memoryName = "MyTestBuffer";
    size_t bufferSize = 1024;

    std::cout << "Creating Shared Memory" << std::endl;
    HANDLE handle = sharedMem.CreateSharedMemory(memoryName, bufferSize);
    
    std::cout << "\n Opening Shared Memory" << std::endl;
    handle = sharedMem.OpenSharedMemory(memoryName);
    if (handle != NULL) {
        std::cout << "Successfully opened shared memory!" << std::endl;
    }
    
    std::cout << "\n Cleaning Up Shared Memory" << std::endl;
    sharedMem.CleanupSharedMemory(memoryName);
    
    std::cout << "\n Final State" << std::endl;
    std::cout << "Head: " << sharedMem.head.load() << std::endl;
    std::cout << "Tail: " << sharedMem.tail.load() << std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(5));

    
    std::cout << "Program finished!" << std::endl;
    return 0;
}