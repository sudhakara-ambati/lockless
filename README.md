# Lockless

A high-performance lock-free circular queue for Windows inter-process communication using shared memory and C++11 atomics.

## Overview

This project implements a single-producer single-consumer (SPSC) lock-free queue that uses atomic operations instead of mutexes for thread synchronization. The queue uses Windows shared memory for efficient inter-process communication.

## Features

- Lock-free operations using C++11 atomics
- Windows shared memory for IPC
- Single producer, single consumer design
- Fixed-size circular buffer
- Type-safe with element size validation

## Quick Example

Producer Process:

#include "lockless.h"

SharedMemory<int> queue;
std::string name = "MyQueue";
size_t size = 4096;

queue.CreateSharedMemory(name, size);
queue.EnqueueData(42);
queue.CleanupSharedMemory();

Consumer Process:
```cpp
    #include "lockless.h"
    
    SharedMemory<int> queue;
    std::string name = "MyQueue";
    
    queue.OpenSharedMemory(name);
    int value;
    if (queue.ReadData(value)) {
        std::cout << "Received: " << value << "\n";
    }
    queue.CleanupSharedMemory();
```
## Why Lock-Free?

Traditional queues use mutexes which cause context switches and can deadlock. Lock-free queues use atomic compare-and-swap operations that execute at user level without OS involvement, resulting in lower latency and better performance.

## Architecture

The shared memory layout consists of a header with atomic head/tail pointers and a capacity field, followed by the circular buffer data. The producer writes to the head position and the consumer reads from the tail position, with atomic operations ensuring thread safety.

## Building

Requires Windows and a C++17 compiler:
```bash
g++ -std=c++17 -I include -o test1.exe examples/test1.cpp
```
## CI/CD

Automated testing with GitHub Actions:
- Build verification
- Static analysis with clang-tidy
- Memory safety checks
- Code coverage reporting
