#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H
#include <string>
namespace CHTL {
class Loader {
public:
    static std::string loadFile(const std::string& path);
    static bool saveFile(const std::string& path, const std::string& content);
};
} // namespace CHTL
#endif
