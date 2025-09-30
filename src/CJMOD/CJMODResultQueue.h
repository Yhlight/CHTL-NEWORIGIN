#pragma once

#include <string>
#include <vector>
#include <mutex>

// A thread-safe singleton queue to hold the results of CJMOD code generation.
// The static CJMODGenerator::exportResult function will add to this queue,
// and the main CHTLJSCompiler will retrieve the results from it.
class CJMODResultQueue {
public:
    static CJMODResultQueue& getInstance() {
        static CJMODResultQueue instance;
        return instance;
    }

    // Add a generated code snippet to the queue.
    void addResult(const std::string& result) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        results.push_back(result);
    }

    // Retrieve all results and clear the queue.
    std::vector<std::string> popAllResults() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        std::vector<std::string> popped_results;
        results.swap(popped_results); // Efficiently move and clear
        return popped_results;
    }

    // Deleted copy constructor and assignment operator to prevent duplication.
    CJMODResultQueue(CJMODResultQueue const&) = delete;
    void operator=(CJMODResultQueue const&) = delete;

private:
    CJMODResultQueue() = default;
    std::vector<std::string> results;
    std::mutex queue_mutex;
};