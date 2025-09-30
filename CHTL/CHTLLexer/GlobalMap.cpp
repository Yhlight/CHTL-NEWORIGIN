#include "GlobalMap.h"

namespace CHTL {

void GlobalMap::set(const std::string& key, const std::string& value) {
    data[key] = value;
}

std::string GlobalMap::get(const std::string& key) {
    auto it = data.find(key);
    if (it != data.end()) {
        return it->second;
    }
    return ""; // Return empty string if not found
}

} // namespace CHTL