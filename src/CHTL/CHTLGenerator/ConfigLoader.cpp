#include "ConfigLoader.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <fstream>

namespace CHTL {

ConfigLoader::ConfigLoader() {}

bool ConfigLoader::loadFromFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        return false;
    }
    
    String line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != String::npos) {
            String key = line.substr(0, pos);
            String value = line.substr(pos + 1);
            config_[key] = value;
        }
    }
    
    return true;
}

String ConfigLoader::get(const String& key, const String& defaultValue) const {
    auto it = config_.find(key);
    if (it != config_.end()) {
        return it->second;
    }
    return defaultValue;
}

void ConfigLoader::set(const String& key, const String& value) {
    config_[key] = value;
}

bool ConfigLoader::has(const String& key) const {
    return config_.find(key) != config_.end();
}

} // namespace CHTL
