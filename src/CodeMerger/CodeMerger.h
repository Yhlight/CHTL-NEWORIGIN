#pragma once

#include "../Scanner/UnifiedScanner.h"
#include <string>
#include <map>

class CodeMerger {
public:
    std::string merge(const std::string& chtl_output, const std::map<std::string, CodeFragment>& fragments);
};