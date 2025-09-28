#pragma once

#include <string>
#include <map>

class CodeMerger {
public:
    std::string merge(std::string compiled_source, const std::map<std::string, std::string>& placeholder_map);
};