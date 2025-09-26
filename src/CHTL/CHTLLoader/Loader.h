#pragma once

#include <string>
#include <vector>
#include <map>
#include "libzippp.h"

class Loader {
public:
    // Reads the entire content of a file into a string.
    static std::string loadFile(const std::string& path);

    // Loads a .cmod file and returns a map of its file contents.
    static std::map<std::string, std::string> loadCmod(const std::string& path);
};
