#include "UnifiedScanner.h"
#include <cctype>
#include <sstream>
#include <algorithm>

std::string UnifiedScanner::generate_placeholder() {
    std::stringstream ss;
    ss << "/*__CHTL_PLACEHOLDER_" << placeholder_id_counter++ << "__*/";
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

    // This simplified scanner only looks for top-level 'style' blocks.
    // 'script' blocks are ignored and passed through to the CHTL parser.
    while (search_pos < source.length()) {
        size_t style_pos = source.find("style", search_pos);

        if (style_pos == std::string::npos) {
            break; // No more style blocks found
        }

        // Make sure we found a whole word, not part of another identifier
        if (!is_whole_word(source, style_pos, 5)) {
            search_pos = style_pos + 1;
            continue;
        }

        // Check if this is a top-level style block
        int brace_level_before = calculate_brace_level_up_to(source, style_pos);
        if (brace_level_before != 0) {
            search_pos = style_pos + 1;
            continue;
        }

        // Find the opening brace '{'
        size_t open_brace_pos = style_pos + 5;
        while (open_brace_pos < source.length() && isspace(source[open_brace_pos])) {
            open_brace_pos++;
        }

        if (open_brace_pos >= source.length() || source[open_brace_pos] != '{') {
            search_pos = style_pos + 1;
            continue;
        }

        // Append the CHTL part before this block
        result_ss << source.substr(last_pos, style_pos - last_pos);

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
            search_pos = style_pos + 1;
            continue;
        }

        // Extract the content of the block
        std::string block_content = source.substr(block_content_start, end_brace_pos - block_content_start - 1);

        // Store the CSS fragment. No placeholder is inserted into the CHTL stream.
        std::string placeholder = generate_placeholder();
        output.fragments[placeholder] = { FragmentType::CSS, block_content };

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