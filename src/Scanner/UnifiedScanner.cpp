#include "UnifiedScanner.h"
#include <cctype>
#include <algorithm>

namespace {
// Helper to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

int calculate_brace_level_up_to(const std::string& s, size_t end) {
    int level = 0;
    for(size_t i = 0; i < end && i < s.length(); ++i) {
        if (s[i] == '{') level++;
        else if (s[i] == '}') level--;
    }
    return level;
}

} // anonymous namespace

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t last_pos = 0;
    size_t search_pos = 0;

    while (search_pos < source.length()) {
        size_t script_pos = source.find("script", search_pos);
        size_t style_pos = source.find("style", search_pos);

        size_t next_pos = std::string::npos;
        bool is_script = false;
        size_t keyword_len = 0;

        if (script_pos != std::string::npos && (style_pos == std::string::npos || script_pos < style_pos)) {
            next_pos = script_pos;
            is_script = true;
            keyword_len = 6;
        } else if (style_pos != std::string::npos) {
            next_pos = style_pos;
            is_script = false;
            keyword_len = 5;
        } else {
            break; // No more keywords
        }

        // Validate it's a whole word
        bool is_whole_word = (next_pos == 0 || !isalnum(source[next_pos - 1])) &&
                             (next_pos + keyword_len >= source.length() || !isalnum(source[next_pos + keyword_len]));
        if (!is_whole_word) {
            search_pos = next_pos + 1;
            continue;
        }

        // Find the opening brace
        size_t open_brace_pos = next_pos + keyword_len;
        while(open_brace_pos < source.length() && isspace(source[open_brace_pos])) {
            open_brace_pos++;
        }

        if (open_brace_pos >= source.length() || source[open_brace_pos] != '{') {
            search_pos = next_pos + 1;
            continue;
        }

        // Check if the block is top-level. The scanner should only separate global blocks.
        int brace_level_before = calculate_brace_level_up_to(source, next_pos);
        if (brace_level_before != 0) {
             search_pos = next_pos + 1;
             continue;
        }

        // A valid block is found.
        // Add the preceding CHTL fragment.
        if (next_pos > last_pos) {
            std::string content = source.substr(last_pos, next_pos - last_pos);
            if (!trim(content).empty()) {
                fragments.push_back({FragmentType::CHTL, content});
            }
        }

        // Find matching closing brace
        size_t block_content_start = open_brace_pos + 1;
        size_t end_brace_pos = block_content_start;
        int block_brace_level = 1;
        while (block_brace_level > 0 && end_brace_pos < source.length()) {
            if (source[end_brace_pos] == '{') block_brace_level++;
            else if (source[end_brace_pos] == '}') block_brace_level--;
            end_brace_pos++;
        }

        if (block_brace_level != 0) { // Unterminated block
            search_pos = next_pos + 1;
            continue;
        }

        std::string blockContent = source.substr(block_content_start, end_brace_pos - block_content_start - 1);
        bool hasChtlFeatures = !is_script && (blockContent.find("# ") != std::string::npos);

        if (hasChtlFeatures) {
            // This style block contains CHTL features. Treat the whole block as CHTL.
            // We need to merge it with the previous CHTL fragment if possible.
            std::string fullBlock = source.substr(next_pos, end_brace_pos - next_pos);
            if (!fragments.empty() && fragments.back().type == FragmentType::CHTL) {
                fragments.back().content += fullBlock;
            } else {
                fragments.push_back({FragmentType::CHTL, fullBlock});
            }
        } else {
            // Original behavior: treat as a pure CSS or JS block.
            fragments.push_back({
                is_script ? FragmentType::JS : FragmentType::CSS,
                blockContent
            });
        }

        last_pos = end_brace_pos;
        search_pos = last_pos;
    }

    if (last_pos < source.length()) {
        std::string content = source.substr(last_pos);
        if (!trim(content).empty()) {
            fragments.push_back({FragmentType::CHTL, content});
        }
    }

    if (fragments.empty() && !source.empty()) {
        fragments.push_back({FragmentType::CHTL, source});
    }

    return fragments;
}