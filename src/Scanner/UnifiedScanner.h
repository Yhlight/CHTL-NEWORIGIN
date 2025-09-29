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
    // Parsing state to handle nested contexts like strings and comments
    enum class ParseState {
        NORMAL,
        IN_SINGLE_QUOTE_STRING,
        IN_DOUBLE_QUOTE_STRING,
        IN_TEMPLATE_LITERAL,
        IN_LINE_COMMENT,
        IN_BLOCK_COMMENT
    };

    int placeholder_id_counter = 0;

    // Helper to find the end of a block with context awareness
    size_t find_matching_brace(const std::string& s, size_t start_pos);

    // Identifies if a script block contains CHTL JS features
    CodeFragment processScriptBlock(const std::string& content);
};