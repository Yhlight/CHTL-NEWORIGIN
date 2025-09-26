#include "Loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string Loader::loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::map<std::string, std::string> Loader::loadCmod(const std::string& path) {
    libzippp::ZipArchive zf(path);
    if (!zf.open(libzippp::ZipArchive::ReadOnly)) {
        throw std::runtime_error("Failed to open CMOD archive: " + path);
    }

    std::map<std::string, std::string> fileContents;
    auto entries = zf.getEntries();
    for (const auto& entry : entries) {
        if (!entry.isDirectory()) {
            void* binaryData = entry.readAsBinary();
            if (binaryData) {
                std::string content(static_cast<char*>(binaryData), entry.getSize());
                fileContents[entry.getName()] = content;
                delete[] static_cast<char*>(binaryData);
            }
        }
    }

    zf.close();
    return fileContents;
}
