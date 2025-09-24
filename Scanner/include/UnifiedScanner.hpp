#pragma once

#include <string>
#include <vector>

namespace chtl::scanner {

enum class FragmentType {
    Chtl,
    ChtlJs,
    CssPlaceholder,
    JsPlaceholder
};

struct Fragment {
    FragmentType type;
    std::string content;
};

class UnifiedScanner {
public:
    std::vector<Fragment> scan(const std::string& source) const;
};

}
