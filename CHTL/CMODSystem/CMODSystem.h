#pragma once

#include <string>

namespace CHTL {

class CMODSystem {
public:
    static bool pack(const std::string& modulePath, const std::string& outputPath);
    static bool unpack(const std::string& cmodPath, const std::string& extractPath);
};

} // namespace CHTL
