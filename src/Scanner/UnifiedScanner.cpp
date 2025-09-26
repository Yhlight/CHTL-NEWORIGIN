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

// New method to scan inside script blocks for CHTL JS syntax
std::vector<CodeFragment> UnifiedScanner::scan_script_fragment(const std::string& script_source) {
    std::vector<CodeFragment> script_fragments;
    size_t last_pos = 0;
    size_t search_pos = 0;

    while (search_pos < script_source.length()) {
        size_t open_delim_pos = script_source.find("{{", search_pos);
        if (open_delim_pos == std::string::npos) {
            break; // No more CHTL JS
        }

        size_t close_delim_pos = script_source.find("}}", open_delim_pos);
        if (close_delim_pos == std::string::npos) {
            break; // Unterminated CHTL JS
        }

        // Add the preceding JS fragment if it's not just whitespace
        if (open_delim_pos > last_pos) {
            std::string js_content = trim(script_source.substr(last_pos, open_delim_pos - last_pos));
            if (!js_content.empty()) {
                script_fragments.push_back({FragmentType::JS, js_content});
            }
        }

        // Add the CHTL_JS fragment
        script_fragments.push_back({FragmentType::CHTL_JS, script_source.substr(open_delim_pos, close_delim_pos - open_delim_pos + 2)});

        last_pos = close_delim_pos + 2;
        search_pos = last_pos;
    }

    // Add any remaining JS fragment if it's not just whitespace
    if (last_pos < script_source.length()) {
        std::string js_content = trim(script_source.substr(last_pos));
        if (!js_content.empty()) {
            script_fragments.push_back({FragmentType::JS, js_content});
        }
    }

    // If no CHTL JS was found, the whole thing is just JS
    if (script_fragments.empty() && !script_source.empty()) {
        script_fragments.push_back({FragmentType::JS, trim(script_source)});
    }

    return script_fragments;
}


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

        // Check if the block is top-level. Only top-level blocks are fragmented.
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

        // Add the block's content
        std::string block_content = source.substr(block_content_start, end_brace_pos - block_content_start - 1);
        if (is_script) {
            auto script_fragments = scan_script_fragment(block_content);
            fragments.insert(fragments.end(), script_fragments.begin(), script_fragments.end());
        } else {
            fragments.push_back({FragmentType::CSS, block_content});
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