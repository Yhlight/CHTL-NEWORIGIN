#pragma once

#include <chrono>
#include <string>
#include <functional>

namespace CHTL {
namespace CLI {

class CompilerMonitor {
public:
    CompilerMonitor();
    ~CompilerMonitor() = default;
    
    // Monitoring configuration
    void setMaxMemoryMB(size_t maxMemoryMB) { maxMemoryMB_ = maxMemoryMB; }
    void setMaxTimeSeconds(size_t maxTimeSeconds) { maxTimeSeconds_ = maxTimeSeconds; }
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // Monitoring control
    void start();
    void stop();
    void reset();
    
    // Status queries
    bool isRunning() const { return running_; }
    bool hasExceededLimits() const;
    std::string getStatus() const;
    
    // Memory monitoring
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    
    // Time monitoring
    std::chrono::milliseconds getElapsedTime() const;
    std::chrono::milliseconds getPeakTime() const;
    
    // Callbacks
    void setOnMemoryLimitExceeded(std::function<void()> callback) { onMemoryLimitExceeded_ = callback; }
    void setOnTimeLimitExceeded(std::function<void()> callback) { onTimeLimitExceeded_ = callback; }
    void setOnLimitExceeded(std::function<void()> callback) { onLimitExceeded_ = callback; }
    
    // Statistics
    struct Statistics {
        size_t maxMemoryMB;
        size_t maxTimeSeconds;
        size_t currentMemoryMB;
        std::chrono::milliseconds elapsedTime;
        bool limitsExceeded;
    };
    
    Statistics getStatistics() const;
    
private:
    bool running_;
    bool verbose_;
    size_t maxMemoryMB_;
    size_t maxTimeSeconds_;
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point peakTime_;
    
    std::function<void()> onMemoryLimitExceeded_;
    std::function<void()> onTimeLimitExceeded_;
    std::function<void()> onLimitExceeded_;
    
    void checkLimits();
    size_t getCurrentMemoryUsageMB() const;
};

} // namespace CLI
} // namespace CHTL