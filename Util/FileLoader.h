#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {
    inline std::string load_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}
