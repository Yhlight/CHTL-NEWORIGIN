#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {
namespace FileSystem {

std::string readFileToString(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

bool writeFileFromString(const std::string& filePath, const std::string& content) {
    std::ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
        return false;
    }
    outputFile << content;
    return true;
}

} // namespace FileSystem
} // namespace CHTL
