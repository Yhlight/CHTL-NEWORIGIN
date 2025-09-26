#include "UnifiedScanner.h"
#include <cctype>
#include <sstream>
#include <algorithm>

std::string UnifiedScanner::generate_placeholder() {
    std::stringstream ss;
    ss << "chtl_placeholder_" << placeholder_id_counter++;
    return ss.str();
}

namespace {
// Helper to check if a keyword is a whole word at a given position
bool is_whole_word(const std::string& source, size_t pos, size_t len) {
    bool start_ok = (pos == 0) || !isalnum(source[pos - 1]);
    bool end_ok = (pos + len >= source.length()) || !isalnum(source[pos + len]);
    return start_ok && end_ok;
}

// Helper to find the level of nested braces up to a certain point in the code
int calculate_brace_level_up_to(const std::string& s, size_t end) {
    int level = 0;
    for(size_t i = 0; i < end && i < s.length(); ++i) {
        if (s[i] == '{') level++;
        else if (s[i] == '}') level--;
    }
    return level;
}

} // anonymous namespace


ScannedOutput UnifiedScanner::scan(const std::string& source) {
    ScannedOutput output;
    std::stringstream result_ss;
    size_t last_pos = 0;
    size_t search_pos = 0;
    placeholder_id_counter = 0; // Reset for each scan

    while (search_pos < source.length()) {
        size_t script_pos = source.find("script", search_pos);
        size_t style_pos = source.find("style", search_pos);

        size_t next_pos = std::string::npos;
        bool is_script = false;
        size_t keyword_len = 0;

        // Find the next occurrence of either "script" or "style"
        if (script_pos != std::string::npos && (style_pos == std::string::npos || script_pos < style_pos)) {
            next_pos = script_pos;
            is_script = true;
            keyword_len = 6;
        } else if (style_pos != std::string::npos) {
            next_pos = style_pos;
            is_script = false;
            keyword_len = 5;
        } else {
            break; // No more keywords found
        }

        // Make sure we found a whole word, not part of another identifier
        if (!is_whole_word(source, next_pos, keyword_len)) {
            search_pos = next_pos + 1;
            continue;
        }

        // Find the opening brace '{'
        size_t open_brace_pos = next_pos + keyword_len;
        while (open_brace_pos < source.length() && isspace(source[open_brace_pos])) {
            open_brace_pos++;
        }

        if (open_brace_pos >= source.length() || source[open_brace_pos] != '{') {
            search_pos = next_pos + 1;
            continue;
        }

        // Only consider top-level style blocks for separation
        if (!is_script) {
            int brace_level_before = calculate_brace_level_up_to(source, next_pos);
            if (brace_level_before != 0) {
                search_pos = next_pos + 1;
                continue;
            }
        }

        // Append the CHTL part before this block
        result_ss << source.substr(last_pos, next_pos - last_pos);

        // Find the matching closing brace '}'
        size_t block_content_start = open_brace_pos + 1;
        size_t end_brace_pos = block_content_start;
        int brace_level = 1;
        while (brace_level > 0 && end_brace_pos < source.length()) {
            if (source[end_brace_pos] == '{') brace_level++;
            else if (source[end_brace_pos] == '}') brace_level--;
            end_brace_pos++;
        }

        if (brace_level != 0) { // Unterminated block, treat as CHTL
            search_pos = next_pos + 1;
            continue;
        }

        // Extract the content of the block
        std::string block_content = source.substr(block_content_start, end_brace_pos - block_content_start - 1);

        // Generate a placeholder and store the fragment
        std::string placeholder = generate_placeholder();
        result_ss << placeholder << "{}"; // Create an empty CHTL element as a placeholder
        output.fragments[placeholder] = {
            is_script ? FragmentType::CHTL_JS : FragmentType::CSS,
            block_content
        };

        last_pos = end_brace_pos;
        search_pos = last_pos;
    }

    // Append any remaining CHTL content
    if (last_pos < source.length()) {
        result_ss << source.substr(last_pos);
    }

    output.chtl_with_placeholders = result_ss.str();

    // If no placeholders were made, the whole thing is CHTL.
    if (output.fragments.empty() && !source.empty()) {
        output.chtl_with_placeholders = source;
    }

    return output;
}