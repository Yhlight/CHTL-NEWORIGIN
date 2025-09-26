#pragma once

#include <string>
#include <vector>
#include <map>

enum class FragmentType {
    CHTL,
    CSS,
    CHTL_JS,
    JS
};

struct CodeFragment {
    FragmentType type;
    std::string content;
};

class UnifiedScanner {
public:
    std::vector<CodeFragment> scan(const std::string& source);

private:
    // Helper method to scan inside a script block
    std::vector<CodeFragment> scan_script_fragment(const std::string& script_source);
};