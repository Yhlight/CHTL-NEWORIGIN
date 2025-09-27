#include "UnifiedScanner.h"
#include <string>
#include <cctype>

// Helper to check if a character is part of a valid identifier.
bool is_word_character(char c) {
    return isalnum(c) || c == '_';
}

// Helper to check if a substring at a position is a keyword, ensuring it's a whole word.
bool is_keyword_at(const std::string& s, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > s.length()) {
        return false;
    }
    if (s.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    if (pos > 0 && is_word_character(s[pos - 1])) {
        return false; // The character before is part of a larger word.
    }
    if (pos + keyword.length() < s.length() && is_word_character(s[pos + keyword.length()])) {
        return false; // The character after is part of a larger word.
    }
    return true;
}

#include <vector>

// New method to analyze script content
CodeFragment UnifiedScanner::processScriptBlock(const std::string& content) {
    // A more robust, state-aware heuristic to differentiate CHTL JS from plain JS.
    enum class ScanState {
        NORMAL,
        IN_SINGLE_LINE_COMMENT,
        IN_MULTI_LINE_COMMENT,
        IN_STRING
    };

    ScanState state = ScanState::NORMAL;
    char string_char = 0;

    // Quick check for the most obvious CHTL JS tokens that can't be mistaken in JS.
    // These are unlikely to appear in comments/strings in a way that would fool the parser.
    if (content.find("{{") != std::string::npos || content.find("}}") != std::string::npos || content.find("->") != std::string::npos) {
        return {FragmentType::CHTL_JS, content};
    }

    // More careful check for keywords that could appear in strings/comments.
    const std::vector<std::string> keywords = {"Listen", "Delegate", "Animate", "Router", "Vir", "ScriptLoader"};

    for (size_t i = 0; i < content.length(); ++i) {
        switch (state) {
            case ScanState::NORMAL:
                if (content[i] == '/' && i + 1 < content.length()) {
                    if (content[i+1] == '/') {
                        state = ScanState::IN_SINGLE_LINE_COMMENT;
                        i++;
                    } else if (content[i+1] == '*') {
                        state = ScanState::IN_MULTI_LINE_COMMENT;
                        i++;
                    }
                } else if (content[i] == '\'' || content[i] == '"' || content[i] == '`') {
                    state = ScanState::IN_STRING;
                    string_char = content[i];
                } else {
                    // Check for CHTL JS keywords only when not in a comment or string.
                    for (const auto& keyword : keywords) {
                        if (is_keyword_at(content, i, keyword)) {
                            return {FragmentType::CHTL_JS, content};
                        }
                    }
                }
                break;
            case ScanState::IN_SINGLE_LINE_COMMENT:
                if (content[i] == '\n') {
                    state = ScanState::NORMAL;
                }
                break;
            case ScanState::IN_MULTI_LINE_COMMENT:
                if (content[i] == '*' && i + 1 < content.length() && content[i+1] == '/') {
                    state = ScanState::NORMAL;
                    i++;
                }
                break;
            case ScanState::IN_STRING:
                if (content[i] == '\\' && i + 1 < content.length()) {
                    i++; // skip escaped character
                } else if (content[i] == string_char) {
                    state = ScanState::NORMAL;
                }
                break;
        }
    }

    // If we get through the whole string without finding a valid CHTL JS keyword, it's plain JS.
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