#pragma once

#include <string>
#include <map>
#include <vector>

namespace CHTL {

enum class FragmentType {
    CHTL_MAIN,
    CSS_GLOBAL,
    JS_PLAIN,
    CHTL_JS
};

struct CodeFragment {
    FragmentType type;
    std::string content;
};

struct ScanResult {
    // The main CHTL source, with all CSS and script content replaced by placeholders.
    std::string source_with_placeholders;
    // A map from placeholder to the code fragment it represents.
    std::map<std::string, CodeFragment> fragments;
};

} // namespace CHTL
