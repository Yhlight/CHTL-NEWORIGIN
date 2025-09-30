#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <map>
#include <string>

namespace CHTL {

class GlobalMap {
public:
    // Basic placeholder for a global map functionality
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);

private:
    std::map<std::string, std::string> data;
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H