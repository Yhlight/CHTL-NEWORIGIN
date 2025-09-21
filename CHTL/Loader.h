#pragma once

#include <string>
#include <filesystem>

namespace CHTL {

class Loader {
public:
    Loader() = default;
    std::string loadSource(const std::filesystem::path& path);
    std::string loadFromCMOD(const std::filesystem::path& cmodPath, const std::string& internalPath);
};

} // namespace CHTL
