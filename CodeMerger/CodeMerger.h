#pragma once
#include <string>
#include <vector>

namespace codemerger {

struct MergeInput {
    std::vector<std::string> chtl;            // processed CHTL outputs (future)
    std::vector<std::string> chtljs;          // processed CHTL JS outputs (future)
    std::vector<std::string> cssPlaceholders; // "CSS_CODE_PLACEHOLDER_"
    std::vector<std::string> jsPlaceholders;  // "JS_CODE_PLACEHOLDER_"
};

struct MergeOutput {
    std::string html;
    std::string css;
    std::string js;
};

class CodeMerger {
public:
    MergeOutput merge(const MergeInput& in);
};

}

