#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H
#include <string>
#include <unordered_map>
namespace CHTL {
class GlobalMap {
public:
    static GlobalMap& getInstance();
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
private:
    GlobalMap() = default;
    std::unordered_map<std::string, std::string> data_;
};
} // namespace CHTL
#endif
