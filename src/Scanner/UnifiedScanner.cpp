#include "UnifiedScanner.h"
#include <cctype>
#include <algorithm>
#include <vector>

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

// Function to check if a script fragment contains CHTL JS syntax
bool isCHTLJS(const std::string& script_content) {
    // A simple heuristic: check for syntax unique to CHTL JS
    const std::vector<std::string> chtl_js_tokens = {
        "->", "{{", "Listen", "Delegate", "Animate", "Vir", "Router", "$", "ScriptLoader"
    };

    for (const auto& token : chtl_js_tokens) {
        if (script_content.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // anonymous namespace

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    size_t cursor = 0;
    size_t last_pos = 0;
    int brace_level = 0;

    while (cursor < source.length()) {
        // 1. Skip comments and strings to avoid matching keywords inside them
        if (source[cursor] == '"' || source[cursor] == '\'') {
            char quote_char = source[cursor];
            cursor++;
            while (cursor < source.length()) {
                if (source[cursor] == quote_char && source[cursor - 1] != '\\') break;
                cursor++;
            }
            if (cursor < source.length()) {
                cursor++; // move past closing quote
            }
            continue;
        }
        if (source[cursor] == '/' && cursor + 1 < source.length()) {
            if (source[cursor + 1] == '/') { // Single-line comment
                cursor += 2;
                while (cursor < source.length() && source[cursor] != '\n') {
                    cursor++;
                }
                continue;
            }
            if (source[cursor + 1] == '*') { // Multi-line comment
                cursor += 2;
                while (cursor + 1 < source.length() && (source[cursor] != '*' || source[cursor + 1] != '/')) {
                    cursor++;
                }
                if (cursor + 1 < source.length()) {
                    cursor += 2; // move past */
                }
                continue;
            }
        }

        // 2. Track brace level for context
        if (source[cursor] == '{') {
            brace_level++;
        } else if (source[cursor] == '}') {
            if (brace_level > 0) {
                brace_level--;
            }
        }

        // 3. Check for top-level 'style' or 'script' keywords
        if (brace_level == 0 && (source.substr(cursor, 5) == "style" || source.substr(cursor, 6) == "script")) {
            bool is_script = source.substr(cursor, 6) == "script";
            size_t keyword_len = is_script ? 6 : 5;

            bool is_whole_word = (cursor == 0 || !isalnum(source[cursor - 1])) &&
                                 (cursor + keyword_len >= source.length() || !isalnum(source[cursor + keyword_len]));

            if (is_whole_word) {
                size_t open_brace_pos = source.find('{', cursor + keyword_len);
                bool whitespace_only = true;
                if (open_brace_pos != std::string::npos) {
                    for(size_t i = cursor + keyword_len; i < open_brace_pos; ++i) {
                        if (!isspace(source[i])) {
                            whitespace_only = false;
                            break;
                        }
                    }
                }

                if (open_brace_pos != std::string::npos && whitespace_only) {
                    // Found a valid top-level block.
                    if (cursor > last_pos) {
                        std::string content = source.substr(last_pos, cursor - last_pos);
                        if (!trim(content).empty()) {
                            fragments.push_back({FragmentType::CHTL, content});
                        }
                    }

                    // Find matching closing brace to extract the block content.
                    size_t block_content_start = open_brace_pos + 1;
                    size_t end_brace_pos = block_content_start;
                    int block_brace_level = 1;
                    while (block_brace_level > 0 && end_brace_pos < source.length()) {
                        if (source[end_brace_pos] == '{') block_brace_level++;
                        else if (source[end_brace_pos] == '}') block_brace_level--;
                        end_brace_pos++;
                    }

                    if (block_brace_level == 0) {
                        std::string block_content = source.substr(block_content_start, end_brace_pos - block_content_start - 1);
                        FragmentType type;
                        if (is_script) {
                            type = isCHTLJS(block_content) ? FragmentType::CHTL_JS : FragmentType::JS;
                        } else {
                            type = FragmentType::CSS;
                        }

                        fragments.push_back({type, block_content});
                        cursor = end_brace_pos;
                        last_pos = cursor;
                        brace_level = 0; // Reset brace level after block
                        continue;
                    }
                }
            }
        }

        cursor++;
    }

    // Add any remaining CHTL fragment at the end of the file.
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