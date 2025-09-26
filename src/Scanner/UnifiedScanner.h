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

struct ScannedOutput {
    std::string chtl_with_placeholders;
    std::map<std::string, CodeFragment> fragments;
};

class UnifiedScanner {
public:
    ScannedOutput scan(const std::string& source);

private:
    int placeholder_id_counter = 0;
    std::string generate_placeholder();
};