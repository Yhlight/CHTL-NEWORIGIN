#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class CHTLLoader {
public:
    static std::string loadFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

#endif // CHTL_LOADER_H