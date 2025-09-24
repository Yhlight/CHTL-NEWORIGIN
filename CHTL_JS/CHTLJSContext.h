#pragma once
#include <string>
#include <vector>

namespace chtljs {

struct Fragment {
    enum class Type { CHTL_JS, JS_PLACEHOLDER };
    Type type { Type::CHTL_JS };
    std::string content;
};

struct Context {
    std::vector<Fragment> fragments;
};

}

