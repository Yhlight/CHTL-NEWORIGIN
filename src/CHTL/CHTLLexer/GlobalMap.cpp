#include "GlobalMap.h"
namespace CHTL {
GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}
void GlobalMap::set(const std::string& key, const std::string& value) {
    data_[key] = value;
}
std::string GlobalMap::get(const std::string& key) const {
    auto it = data_.find(key);
    return (it != data_.end()) ? it->second : "";
}
}
