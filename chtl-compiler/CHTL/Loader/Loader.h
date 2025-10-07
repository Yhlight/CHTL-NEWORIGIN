#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm> // For std::find

class Loader {
public:
    Loader(const std::string& projectRoot = ".") : projectRoot(projectRoot) {}

    std::string loadFile(const std::string& filepath) {
        std::string absolute_path = (filepath[0] == '/') ? filepath : projectRoot + "/" + filepath;

        if (std::find(importStack.begin(), importStack.end(), absolute_path) != importStack.end()) {
            std::string stack_str;
            for(const auto& file : importStack) {
                stack_str += file + " -> ";
            }
            stack_str += absolute_path;
            throw std::runtime_error("Circular dependency detected: " + stack_str);
        }

        importStack.push_back(absolute_path);

        std::ifstream file(absolute_path);
        if (!file.is_open()) {
            importStack.pop_back();
            throw std::runtime_error("Could not open file: " + absolute_path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void finishFile(const std::string& filepath) {
         if (!importStack.empty() && importStack.back() == filepath) {
            importStack.pop_back();
        }
    }

private:
    std::string projectRoot;
    std::vector<std::string> importStack;
};