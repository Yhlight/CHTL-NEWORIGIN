#ifndef CHTL_FILE_WATCHER_H
#define CHTL_FILE_WATCHER_H

#include "../../Common.h"
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <sys/stat.h>

namespace CHTL {

enum class FileEvent {
    Modified,
    Created,
    Deleted
};

using FileCallback = std::function<void(const String&, FileEvent)>;

class FileWatcher {
public:
    FileWatcher() : running_(false) {}
    
    void watch(const String& path, FileCallback callback) {
        watched_files_[path] = {callback, getLastModified(path)};
    }
    
    void start() {
        running_ = true;
        watch_thread_ = std::thread([this]() {
            while (running_) {
                checkFiles();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    void stop() {
        running_ = false;
        if (watch_thread_.joinable()) {
            watch_thread_.join();
        }
    }
    
    ~FileWatcher() {
        stop();
    }
    
private:
    struct WatchInfo {
        FileCallback callback;
        std::time_t lastModified;
    };
    
    std::atomic<bool> running_;
    std::thread watch_thread_;
    HashMap<String, WatchInfo> watched_files_;
    
    std::time_t getLastModified(const String& path) {
        struct stat st;
        if (stat(path.c_str(), &st) == 0) {
            return st.st_mtime;
        }
        return 0;
    }
    
    void checkFiles() {
        for (auto& [path, info] : watched_files_) {
            auto current = getLastModified(path);
            if (current == 0 && info.lastModified != 0) {
                info.callback(path, FileEvent::Deleted);
                info.lastModified = 0;
            } else if (current != 0 && info.lastModified == 0) {
                info.callback(path, FileEvent::Created);
                info.lastModified = current;
            } else if (current != info.lastModified && current != 0) {
                info.callback(path, FileEvent::Modified);
                info.lastModified = current;
            }
        }
    }
};

} // namespace CHTL

#endif // CHTL_FILE_WATCHER_H
