#include "VirRegistry.h"
#include <sstream>

namespace CHTL {
namespace JS {

VirRegistry& VirRegistry::getInstance() {
    static VirRegistry instance;
    return instance;
}

void VirRegistry::registerVir(const String& name, const VirObjectInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    registry_[name] = info;
}

bool VirRegistry::hasVir(const String& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return registry_.find(name) != registry_.end();
}

Optional<VirObjectInfo> VirRegistry::getVir(const String& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = registry_.find(name);
    if (it != registry_.end()) {
        return it->second;
    }
    return std::nullopt;
}

Vector<String> VirRegistry::getAllVirNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    Vector<String> names;
    for (const auto& pair : registry_) {
        names.push_back(pair.first);
    }
    return names;
}

Vector<VirObjectInfo> VirRegistry::getAllVirs() const {
    std::lock_guard<std::mutex> lock(mutex_);
    Vector<VirObjectInfo> virs;
    for (const auto& pair : registry_) {
        virs.push_back(pair.second);
    }
    return virs;
}

void VirRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    registry_.clear();
}

String VirRegistry::generateJSONManifest() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"virs\": [\n";
    
    bool first = true;
    for (const auto& pair : registry_) {
        if (!first) {
            json << ",\n";
        }
        first = false;
        
        const auto& info = pair.second;
        json << "    {\n";
        json << "      \"name\": \"" << info.name << "\",\n";
        json << "      \"type\": \"" << info.type << "\",\n";
        json << "      \"hasVirtualObject\": " << (info.hasVirtualObject ? "true" : "false");
        
        if (info.hasVirtualObject && !info.virtualKeys.empty()) {
            json << ",\n";
            json << "      \"virtualKeys\": [";
            for (size_t i = 0; i < info.virtualKeys.size(); i++) {
                if (i > 0) json << ", ";
                json << "\"" << info.virtualKeys[i] << "\"";
            }
            json << "]";
        }
        
        json << "\n    }";
    }
    
    json << "\n  ],\n";
    json << "  \"count\": " << registry_.size() << "\n";
    json << "}";
    
    return json.str();
}

} // namespace JS
} // namespace CHTL
