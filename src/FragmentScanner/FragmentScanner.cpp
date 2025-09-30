#include "FragmentScanner.h"
#include <stdexcept>

// A simplified helper to find the closing delimiter for a block.
// This is a placeholder for a more robust, context-aware implementation.
size_t find_closing_delimiter(const std::string& content, size_t start_pos, char open_delim, char close_delim) {
    int balance = 1;
    for (size_t i = start_pos + 1; i < content.length(); ++i) {
        if (content[i] == open_delim) {
            balance++;
        } else if (content[i] == close_delim) {
            balance--;
            if (balance == 0) {
                return i;
            }
        }
    }
    return std::string::npos; // Not found
}

FragmentScanner::FragmentScanner(const std::string& source) : source(source) {}

std::vector<CodeFragment> FragmentScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t last_pos = 0;

    while (position < source.length()) {
        // Look for the start of a style or script block
        size_t style_pos = source.find("style", position);
        size_t script_pos = source.find("script", position);

        size_t next_block_pos = std::string::npos;
        FragmentType next_block_type = FragmentType::CHTL;

        if (style_pos != std::string::npos && (script_pos == std::string::npos || style_pos < script_pos)) {
            next_block_pos = style_pos;
            next_block_type = FragmentType::CSS;
        } else if (script_pos != std::string::npos) {
            next_block_pos = script_pos;
            next_block_type = FragmentType::JS;
        }

        if (next_block_pos == std::string::npos) {
            // No more special blocks, the rest is CHTL
            if (last_pos < source.length()) {
                fragments.push_back({FragmentType::CHTL, source.substr(last_pos)});
            }
            break;
        }

        // Add the CHTL content before the block
        if (next_block_pos > last_pos) {
            fragments.push_back({FragmentType::CHTL, source.substr(last_pos, next_block_pos - last_pos)});
        }

        // Find the opening brace of the block
        size_t block_start = source.find('{', next_block_pos);
        if (block_start == std::string::npos) {
            throw std::runtime_error("Malformed block: missing opening brace.");
        }

        // Find the closing brace of the block
        size_t block_end = find_closing_delimiter(source, block_start, '{', '}');
        if (block_end == std::string::npos) {
            throw std::runtime_error("Malformed block: missing closing brace.");
        }

        // Extract the content of the block (inside the braces)
        std::string block_content = source.substr(block_start + 1, block_end - block_start - 1);
        fragments.push_back({next_block_type, block_content});

        // Move past the processed block
        position = block_end + 1;
        last_pos = position;
    }

    return fragments;
}