#pragma once

#include <string>

class Loader {
public:
    // Reads the entire content of a file into a string.
    // Throws a runtime_error if the file cannot be opened.
    static std::string loadFile(const std::string& path);
};
