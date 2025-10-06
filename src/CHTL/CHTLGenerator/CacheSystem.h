#ifndef CHTL_CACHE_SYSTEM_H
#define CHTL_CACHE_SYSTEM_H

#include "../../Common.h"
#include <unordered_map>
#include <chrono>

namespace CHTL {

template<typename KeyType, typename ValueType>
class CacheSystem {
public:
    struct CacheEntry {
        ValueType value;
        std::chrono::system_clock::time_point timestamp;
        size_t accessCount;
        
        CacheEntry() : accessCount(0) {}
        CacheEntry(const ValueType& v) 
            : value(v), timestamp(std::chrono::system_clock::now()), accessCount(1) {}
    };
    
    void set(const KeyType& key, const ValueType& value) {
        cache_[key] = CacheEntry(value);
    }
    
    bool get(const KeyType& key, ValueType& value) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            value = it->second.value;
            it->second.accessCount++;
            it->second.timestamp = std::chrono::system_clock::now();
            hits_++;
            return true;
        }
        misses_++;
        return false;
    }
    
    bool has(const KeyType& key) const {
        return cache_.find(key) != cache_.end();
    }
    
    void invalidate(const KeyType& key) {
        cache_.erase(key);
    }
    
    void clear() {
        cache_.clear();
        hits_ = 0;
        misses_ = 0;
    }
    
    size_t size() const {
        return cache_.size();
    }
    
    double hitRate() const {
        size_t total = hits_ + misses_;
        return total > 0 ? static_cast<double>(hits_) / total : 0.0;
    }
    
    void evictOldest(size_t maxSize) {
        if (cache_.size() <= maxSize) return;
        
        auto oldest = cache_.begin();
        for (auto it = cache_.begin(); it != cache_.end(); ++it) {
            if (it->second.timestamp < oldest->second.timestamp) {
                oldest = it;
            }
        }
        cache_.erase(oldest);
    }
    
private:
    std::unordered_map<KeyType, CacheEntry> cache_;
    size_t hits_ = 0;
    size_t misses_ = 0;
};

} // namespace CHTL

#endif // CHTL_CACHE_SYSTEM_H
