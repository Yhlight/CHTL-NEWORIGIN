#pragma once
#include <string>
#include <vector>
#include <optional>

namespace chtl {

struct Fragment {
    enum class Type { CHTL, CHTL_JS, CSS_PLACEHOLDER, JS_PLACEHOLDER };
    Type type { Type::CHTL };
    std::string content;
};

struct BuildOptions {
    bool inlineAll { false };
    bool inlineCss { false };
    bool inlineJs { false };
    bool defaultStruct { false };
};

struct PipelineContext {
    std::string inputCode;
    std::vector<Fragment> fragments;
    BuildOptions options;
};

} // namespace chtl

