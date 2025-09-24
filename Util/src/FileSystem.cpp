#include "FileSystem.hpp"

#include <fstream>

namespace chtl::util {

std::string readFileToString(const std::string& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) return {};
    std::string data;
    ifs.seekg(0, std::ios::end);
    data.resize(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);
    ifs.read(data.data(), static_cast<std::streamsize>(data.size()));
    return data;
}

bool writeStringToFile(const std::string& path, std::string_view content) {
    std::ofstream ofs(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs) return false;
    ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
    return static_cast<bool>(ofs);
}

}
