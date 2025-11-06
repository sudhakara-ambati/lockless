#include "../include/lockless.h"
#include <iostream>
#include <string>

int main() {
    SharedMemory<int> sharedMem;
    
    std::string memName = "MyTestBuffer";
    size_t size = 4096;

    std::cout << "Creating shared memory..." << std::endl;
    HANDLE h = sharedMem.CreateSharedMemory(memName, size);
    
    if (h == NULL) {
        std::cerr << "Failed!" << std::endl;
        return 1;
    }

    std::cout << "Success! Queue capacity: " << sharedMem.header->capacity << std::endl;

    bool running = true;
    while (running) {
        std::cout << "\n1. Add\n2. Remove\n3. Status\n4. Quit\nChoice: ";
        
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Value: ";
            int val;
            std::cin >> val;
            
            sharedMem.EnqueueData(val);
            std::cout << "Added " << val << std::endl;
            
        } else if (choice == 2) {
            int val;
            if (sharedMem.ReadData(val)) {
                std::cout << "Read: " << val << std::endl;
            } else {
                std::cout << "Empty!" << std::endl;
            }
            
        } else if (choice == 3) {
            size_t h = sharedMem.header->head.load(std::memory_order_acquire);
            size_t t = sharedMem.header->tail.load(std::memory_order_acquire);
            size_t cap = sharedMem.header->capacity;
            
            std::cout << "\nHead: " << h << " Tail: " << t << " Cap: " << cap << std::endl;
            
            size_t count = (h >= t) ? (h - t) : (cap - t + h);
            std::cout << "Items: " << count << std::endl;
            
            // simple visual
            std::cout << "[";
            for (size_t i = 0; i < cap; i++) {
                if (i == h && i == t) std::cout << ".";
                else if (i == h) std::cout << "H";
                else if (i == t) std::cout << "T";
                else if ((t < h && i > t && i < h) || (t > h && (i > t || i < h))) 
                    std::cout << "X";
                else std::cout << "_";
            }
            std::cout << "]" << std::endl;
            
        } else if (choice == 4) {
            running = false;
        }
    }

    sharedMem.CleanupSharedMemory();
    std::cout << "Done" << std::endl;
    
    return 0;
}