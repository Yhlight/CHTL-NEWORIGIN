#include "CompilerMonitor.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <sstream>

namespace CHTL {
namespace CLI {

CompilerMonitor::CompilerMonitor()
    : running_(false)
    , verbose_(false)
    , maxMemoryMB_(1024)  // 1GB default
    , maxTimeSeconds_(300)  // 5 minutes default
    , startTime_(std::chrono::steady_clock::now())
    , peakTime_(std::chrono::steady_clock::now()) {
}

void CompilerMonitor::start() {
    running_ = true;
    startTime_ = std::chrono::steady_clock::now();
    peakTime_ = startTime_;
    
    if (verbose_) {
        std::cout << "Compiler monitoring started" << std::endl;
    }
    
    // Start monitoring thread
    std::thread([this]() {
        while (running_) {
            checkLimits();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
}

void CompilerMonitor::stop() {
    running_ = false;
    
    if (verbose_) {
        auto elapsed = getElapsedTime();
        auto memory = getCurrentMemoryUsage();
        std::cout << "Compiler monitoring stopped - Time: " << elapsed.count() 
                  << "ms, Memory: " << memory << "MB" << std::endl;
    }
}

void CompilerMonitor::reset() {
    running_ = false;
    startTime_ = std::chrono::steady_clock::now();
    peakTime_ = startTime_;
}

bool CompilerMonitor::hasExceededLimits() const {
    auto elapsed = getElapsedTime();
    auto memory = getCurrentMemoryUsage();
    
    return memory > maxMemoryMB_ || 
           std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > maxTimeSeconds_;
}

std::string CompilerMonitor::getStatus() const {
    auto elapsed = getElapsedTime();
    auto memory = getCurrentMemoryUsage();
    
    std::ostringstream oss;
    oss << "Time: " << elapsed.count() << "ms, Memory: " << memory << "MB";
    
    if (hasExceededLimits()) {
        oss << " (LIMITS EXCEEDED)";
    }
    
    return oss.str();
}

size_t CompilerMonitor::getCurrentMemoryUsage() const {
    return getCurrentMemoryUsageMB();
}

size_t CompilerMonitor::getPeakMemoryUsage() const {
    // This would track peak memory usage over time
    return getCurrentMemoryUsageMB();
}

std::chrono::milliseconds CompilerMonitor::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
}

std::chrono::milliseconds CompilerMonitor::getPeakTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(peakTime_ - startTime_);
}

void CompilerMonitor::setOnMemoryLimitExceeded(std::function<void()> callback) {
    onMemoryLimitExceeded_ = callback;
}

void CompilerMonitor::setOnTimeLimitExceeded(std::function<void()> callback) {
    onTimeLimitExceeded_ = callback;
}

void CompilerMonitor::setOnLimitExceeded(std::function<void()> callback) {
    onLimitExceeded_ = callback;
}

CompilerMonitor::Statistics CompilerMonitor::getStatistics() const {
    Statistics stats;
    stats.maxMemoryMB = maxMemoryMB_;
    stats.maxTimeSeconds = maxTimeSeconds_;
    stats.currentMemoryMB = getCurrentMemoryUsage();
    stats.elapsedTime = getElapsedTime();
    stats.limitsExceeded = hasExceededLimits();
    return stats;
}

void CompilerMonitor::checkLimits() {
    if (!running_) return;
    
    auto elapsed = getElapsedTime();
    auto memory = getCurrentMemoryUsage();
    
    bool memoryExceeded = memory > maxMemoryMB_;
    bool timeExceeded = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > maxTimeSeconds_;
    
    if (memoryExceeded && onMemoryLimitExceeded_) {
        onMemoryLimitExceeded_();
    }
    
    if (timeExceeded && onTimeLimitExceeded_) {
        onTimeLimitExceeded_();
    }
    
    if ((memoryExceeded || timeExceeded) && onLimitExceeded_) {
        onLimitExceeded_();
    }
    
    if (verbose_ && (memoryExceeded || timeExceeded)) {
        std::cerr << "WARNING: Compiler limits exceeded - " << getStatus() << std::endl;
    }
}

size_t CompilerMonitor::getCurrentMemoryUsageMB() const {
    // Platform-specific memory usage detection
    // This is a simplified implementation
    
#ifdef _WIN32
    // Windows implementation would use GetProcessMemoryInfo
    return 0;
#elif __linux__
    // Linux implementation would read from /proc/self/status
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::istringstream iss(line.substr(6));
            size_t kb;
            iss >> kb;
            return kb / 1024; // Convert KB to MB
        }
    }
    return 0;
#elif __APPLE__
    // macOS implementation would use task_info
    return 0;
#else
    return 0;
#endif
}

} // namespace CLI
} // namespace CHTL