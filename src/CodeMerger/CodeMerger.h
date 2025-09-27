#pragma once

#include <string>
#include <map>

class CodeMerger {
public:
    std::string merge(const std::string& chtl_output, const std::map<std::string, std::string>& compiled_fragments);
};