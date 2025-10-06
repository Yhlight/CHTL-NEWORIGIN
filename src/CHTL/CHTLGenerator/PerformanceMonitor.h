#ifndef CHTL_PERFORMANCE_MONITOR_H
#define CHTL_PERFORMANCE_MONITOR_H

#include "../../Common.h"
#include <chrono>
#include <unordered_map>
#include <iostream>

namespace CHTL {

class PerformanceMonitor {
public:
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::microseconds;
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void startTimer(const String& name) {
        timers_[name] = std::chrono::high_resolution_clock::now();
    }
    
    Duration stopTimer(const String& name) {
        auto it = timers_.find(name);
        if (it == timers_.end()) {
            return Duration(0);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<Duration>(end - it->second);
        
        // Record duration
        durations_[name] = duration;
        
        // Remove timer
        timers_.erase(it);
        
        return duration;
    }
    
    Duration getDuration(const String& name) const {
        auto it = durations_.find(name);
        if (it == durations_.end()) {
            return Duration(0);
        }
        return it->second;
    }
    
    void printReport() const {
        std::cout << "\n=== Performance Report ===\n";
        for (const auto& [name, duration] : durations_) {
            double ms = duration.count() / 1000.0;
            std::cout << name << ": " << ms << " ms\n";
        }
        std::cout << "==========================\n\n";
    }
    
    void clear() {
        timers_.clear();
        durations_.clear();
    }
    
private:
    PerformanceMonitor() = default;
    std::unordered_map<String, TimePoint> timers_;
    std::unordered_map<String, Duration> durations_;
};

// RAII timer helper
class ScopedTimer {
public:
    explicit ScopedTimer(const String& name) : name_(name) {
        PerformanceMonitor::getInstance().startTimer(name_);
    }
    
    ~ScopedTimer() {
        PerformanceMonitor::getInstance().stopTimer(name_);
    }
    
private:
    String name_;
};

} // namespace CHTL

#endif // CHTL_PERFORMANCE_MONITOR_H
