#include "../include/lockless.h"
#include <iostream>
#include <string>

const size_t BUFFER_SIZE = 4096;

int main() {
    SharedMemory<int> sharedMem;
    
    std::string memName = "MyTestBuffer";
    size_t size = BUFFER_SIZE;

    std::cout << "Creating shared memory..." << std::endl;
    HANDLE handle = sharedMem.CreateSharedMemory(memName, size);
    
    if (handle == NULL) {
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
            size_t head = sharedMem.header->head.load(std::memory_order_acquire);
            size_t tail = sharedMem.header->tail.load(std::memory_order_acquire);
            size_t capacity = sharedMem.header->capacity;
            
            std::cout << "\nHead: " << head << " Tail: " << tail << " Cap: " << capacity << std::endl;
            
            size_t count = (head >= tail) ? (head - tail) : (capacity - tail + head);
            std::cout << "Items: " << count << std::endl;
            
            std::cout << "[";
            for (size_t i = 0; i < capacity; i++) {
                if (i == head && i == tail) {
                    std::cout << ".";
                } else if (i == head) {
                    std::cout << "H";
                } else if (i == tail) {
                    std::cout << "T";
                } else if ((tail < head && i > tail && i < head) || 
                           (tail > head && (i > tail || i < head))) {
                    std::cout << "X";
                } else {
                    std::cout << "_";
                }
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