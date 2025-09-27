#include "UnifiedScanner.h"
#include <string>
#include <cctype>

// Helper to check if a substring at a position is a keyword, ensuring it's a whole word.
bool is_keyword_at(const std::string& s, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > s.length()) {
        return false;
    }
    if (s.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    if (pos > 0 && isalnum(s[pos - 1])) {
        return false; // The character before is alphanumeric, so it's part of a larger word.
    }
    if (pos + keyword.length() < s.length() && isalnum(s[pos + keyword.length()])) {
        return false; // The character after is alphanumeric, so it's part of a larger word.
    }
    return true;
}

// New method to analyze script content
CodeFragment UnifiedScanner::processScriptBlock(const std::string& content) {
    // Heuristic to differentiate CHTL JS from plain JS.
    // Look for CHTL JS-specific syntax.
    if (content.find("{{") != std::string::npos ||
        content.find("}}") != std::string::npos ||
        content.find("->") != std::string::npos ||
        content.find("Listen") != std::string::npos ||
        content.find("Delegate") != std::string::npos ||
        content.find("Animate") != std::string::npos ||
        content.find("Router") != std::string::npos ||
        content.find("Vir") != std::string::npos ||
        content.find("ScriptLoader") != std::string::npos) {
        return {FragmentType::CHTL_JS, content};
    }
    return {FragmentType::JS, content};
}

ScannedOutput UnifiedScanner::scan(const std::string& source) {
    ScannedOutput output;
    std::string placeholder_template = "chtl_placeholder_";
    int placeholder_index = 0;

    std::string result_source;
    size_t last_pos = 0;
    int brace_level = 0;

    for (size_t i = 0; i < source.length(); ++i) {
        bool is_style = is_keyword_at(source, i, "style");
        bool is_script = is_keyword_at(source, i, "script");

        // Check if we found a potential block keyword
        if (is_style || is_script) {
            size_t keyword_len = is_style ? 5 : 6;
            size_t block_start_pos = i + keyword_len;

            // Find the opening brace, skipping whitespace
            while (block_start_pos < source.length() && isspace(source[block_start_pos])) {
                block_start_pos++;
            }

            if (block_start_pos < source.length() && source[block_start_pos] == '{') {
                // We found a block. Now check if we should extract it.
                // Style blocks are only extracted at the top level (brace_level == 0).
                // Script blocks are extracted at any level.
                if ((is_style && brace_level == 0) || is_script) {
                    // Find the matching closing brace
                    int block_brace_level = 1;
                    size_t block_end_pos = block_start_pos + 1;
                    while (block_end_pos < source.length() && block_brace_level > 0) {
                        if (source[block_end_pos] == '{') block_brace_level++;
                        else if (source[block_end_pos] == '}') block_brace_level--;
                        block_end_pos++;
                    }

                    if (block_brace_level == 0) {
                        // Successfully found a complete block to extract.
                        result_source.append(source, last_pos, i - last_pos);

                        std::string placeholder = placeholder_template + std::to_string(placeholder_index++);
                        std::string content = source.substr(block_start_pos + 1, block_end_pos - block_start_pos - 2);

                        if (is_style) {
                            output.fragments[placeholder] = {FragmentType::CSS, content};
                        } else {
                            output.fragments[placeholder] = processScriptBlock(content);
                        }

                        result_source += placeholder + "{}";

                        // Jump the main loop past the extracted block
                        last_pos = block_end_pos;
                        i = block_end_pos - 1; // -1 because the loop will increment i
                        continue;
                    }
                }
            }
        }

        // If we didn't extract a block, just update the nesting level
        if (source[i] == '{') {
            brace_level++;
        } else if (source[i] == '}') {
            if (brace_level > 0) {
                brace_level--;
            }
        }
    }

    // Append any remaining part of the source
    if (last_pos < source.length()) {
        result_source.append(source, last_pos, source.length() - last_pos);
    }

    output.chtl_with_placeholders = result_source;
    return output;
}