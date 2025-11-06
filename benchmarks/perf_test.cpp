#include "../include/lockless.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std::chrono;

struct BenchmarkResult {
    std::string name;
    double opsPerSecond;
    double avgLatencyNs;
    double minLatencyNs;
    double maxLatencyNs;
};

BenchmarkResult benchmarkEnqueue(size_t numOperations) {
    SharedMemory<int> queue;
    std::string name = "BenchQueue";
    size_t size = 1024 * 1024;
    
    HANDLE h = queue.CreateSharedMemory(name, size);
    if (h == nullptr) {
        std::cerr << "Failed to create shared memory\n";
        exit(1);
    }
    
    std::vector<double> latencies;
    latencies.reserve(numOperations);
    
    auto start = high_resolution_clock::now();
    
    for (size_t i = 0; i < numOperations; i++) {
        auto opStart = high_resolution_clock::now();
        queue.EnqueueData(static_cast<int>(i));
        auto opEnd = high_resolution_clock::now();
        
        double latencyNs = duration_cast<nanoseconds>(opEnd - opStart).count();
        latencies.push_back(latencyNs);
    }
    
    auto end = high_resolution_clock::now();
    double totalTimeS = duration_cast<duration<double>>(end - start).count();
    
    double avgLatency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double minLatency = *std::min_element(latencies.begin(), latencies.end());
    double maxLatency = *std::max_element(latencies.begin(), latencies.end());
    double opsPerSec = numOperations / totalTimeS;
    
    queue.CleanupSharedMemory();
    
    return {"Enqueue", opsPerSec, avgLatency, minLatency, maxLatency};
}

BenchmarkResult benchmarkDequeue(size_t numOperations) {
    SharedMemory<int> queue;
    std::string name = "BenchQueue2";
    size_t size = 1024 * 1024;
    
    HANDLE h = queue.CreateSharedMemory(name, size);
    if (h == nullptr) {
        std::cerr << "Failed to create shared memory\n";
        exit(1);
    }
    
    for (size_t i = 0; i < numOperations; i++) {
        queue.EnqueueData(static_cast<int>(i));
    }
    
    std::vector<double> latencies;
    latencies.reserve(numOperations);
    
    auto start = high_resolution_clock::now();
    
    int value;
    for (size_t i = 0; i < numOperations; i++) {
        auto opStart = high_resolution_clock::now();
        queue.ReadData(value);
        auto opEnd = high_resolution_clock::now();
        
        double latencyNs = duration_cast<nanoseconds>(opEnd - opStart).count();
        latencies.push_back(latencyNs);
    }
    
    auto end = high_resolution_clock::now();
    double totalTimeS = duration_cast<duration<double>>(end - start).count();
    
    double avgLatency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double minLatency = *std::min_element(latencies.begin(), latencies.end());
    double maxLatency = *std::max_element(latencies.begin(), latencies.end());
    double opsPerSec = numOperations / totalTimeS;
    
    queue.CleanupSharedMemory();
    
    return {"Dequeue", opsPerSec, avgLatency, minLatency, maxLatency};
}

BenchmarkResult benchmarkRoundTrip(size_t numOperations) {
    SharedMemory<int> queue;
    std::string name = "BenchQueue3";
    size_t size = 1024 * 1024;
    
    HANDLE h = queue.CreateSharedMemory(name, size);
    if (h == nullptr) {
        std::cerr << "Failed to create shared memory\n";
        exit(1);
    }
    
    std::vector<double> latencies;
    latencies.reserve(numOperations);
    
    auto start = high_resolution_clock::now();
    
    int value;
    for (size_t i = 0; i < numOperations; i++) {
        auto opStart = high_resolution_clock::now();
        queue.EnqueueData(static_cast<int>(i));
        queue.ReadData(value);
        auto opEnd = high_resolution_clock::now();
        
        double latencyNs = duration_cast<nanoseconds>(opEnd - opStart).count();
        latencies.push_back(latencyNs);
    }
    
    auto end = high_resolution_clock::now();
    double totalTimeS = duration_cast<duration<double>>(end - start).count();
    
    double avgLatency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double minLatency = *std::min_element(latencies.begin(), latencies.end());
    double maxLatency = *std::max_element(latencies.begin(), latencies.end());
    double opsPerSec = (numOperations * 2) / totalTimeS;
    
    queue.CleanupSharedMemory();
    
    return {"RoundTrip", opsPerSec, avgLatency, minLatency, maxLatency};
}

void printResults(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n=== Lockless Queue Benchmark Results ===\n\n";
    
    for (const auto& result : results) {
        std::cout << result.name << ":\n";
        std::cout << "  Operations/sec: " << std::fixed << std::setprecision(0) 
                  << result.opsPerSecond << "\n";
        std::cout << "  Avg Latency:    " << std::fixed << std::setprecision(2) 
                  << result.avgLatencyNs << " ns\n";
        std::cout << "  Min Latency:    " << std::fixed << std::setprecision(2) 
                  << result.minLatencyNs << " ns\n";
        std::cout << "  Max Latency:    " << std::fixed << std::setprecision(2) 
                  << result.maxLatencyNs << " ns\n";
        std::cout << "\n";
    }
}

void saveResultsJSON(const std::vector<BenchmarkResult>& results) {
    std::ofstream file("benchmark_results.json");
    
    file << "[\n";
    for (size_t i = 0; i < results.size(); i++) {
        const auto& result = results[i];
        file << "  {\n";
        file << "    \"name\": \"" << result.name << "\",\n";
        file << "    \"unit\": \"ops/sec\",\n";
        file << "    \"value\": " << result.opsPerSecond << "\n";
        file << "  }";
        if (i < results.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    
    file.close();
    std::cout << "Results saved to benchmark_results.json\n";
}

int main() {
    const size_t NUM_OPERATIONS = 100000;
    
    std::cout << "Starting benchmarks with " << NUM_OPERATIONS << " operations...\n\n";
    
    std::vector<BenchmarkResult> results;
    
    std::cout << "Running Enqueue benchmark...\n";
    results.push_back(benchmarkEnqueue(NUM_OPERATIONS));
    
    std::cout << "Running Dequeue benchmark...\n";
    results.push_back(benchmarkDequeue(NUM_OPERATIONS));
    
    std::cout << "Running RoundTrip benchmark...\n";
    results.push_back(benchmarkRoundTrip(NUM_OPERATIONS));
    
    printResults(results);
    saveResultsJSON(results);
    
    return 0;
}