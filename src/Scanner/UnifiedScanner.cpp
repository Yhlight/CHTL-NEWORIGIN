#include "UnifiedScanner.h"
#include <string>
#include <cctype>
#include <regex>

namespace {
    bool is_keyword_at(const std::string& s, size_t pos, const std::string& keyword) {
        if (pos + keyword.length() > s.length()) return false;
        if (s.substr(pos, keyword.length()) != keyword) return false;
        if (pos > 0 && (isalnum(s[pos - 1]) || s[pos - 1] == '_')) return false;
        if (pos + keyword.length() < s.length() && (isalnum(s[pos + keyword.length()]) || s[pos + keyword.length()] == '_')) return false;
        return true;
    }
}

size_t UnifiedScanner::find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') return std::string::npos;

    int brace_level = 1;
    ParseState state = ParseState::NORMAL;
    size_t i = start_pos + 1;

    while (i < s.length()) {
        char current = s[i];
        char prev = (i > 0) ? s[i - 1] : '\0';

        switch (state) {
            case ParseState::NORMAL:
                if (current == '{') brace_level++;
                else if (current == '}') {
                    brace_level--;
                    if (brace_level == 0) return i;
                } else if (current == '"') state = ParseState::IN_DOUBLE_QUOTE_STRING;
                else if (current == '\'') state = ParseState::IN_SINGLE_QUOTE_STRING;
                else if (current == '`') state = ParseState::IN_TEMPLATE_LITERAL;
                else if (current == '/' && i + 1 < s.length()) {
                    if (s[i + 1] == '/') state = ParseState::IN_LINE_COMMENT;
                    else if (s[i + 1] == '*') state = ParseState::IN_BLOCK_COMMENT;
                }
                break;
            case ParseState::IN_DOUBLE_QUOTE_STRING: if (current == '"' && prev != '\\') state = ParseState::NORMAL; break;
            case ParseState::IN_SINGLE_QUOTE_STRING: if (current == '\'' && prev != '\\') state = ParseState::NORMAL; break;
            case ParseState::IN_TEMPLATE_LITERAL: if (current == '`' && prev != '\\') state = ParseState::NORMAL; break;
            case ParseState::IN_LINE_COMMENT: if (current == '\n') state = ParseState::NORMAL; break;
            case ParseState::IN_BLOCK_COMMENT: if (current == '/' && prev == '*') state = ParseState::NORMAL; break;
        }
        i++;
    }
    return std::string::npos;
}

CodeFragment UnifiedScanner::processScriptBlock(const std::string& content) {
    if (content.find("{{") != std::string::npos || content.find("}}") != std::string::npos ||
        content.find("->") != std::string::npos || content.find("Listen") != std::string::npos ||
        content.find("Delegate") != std::string::npos || content.find("Animate") != std::string::npos ||
        content.find("Router") != std::string::npos || content.find("Vir") != std::string::npos ||
        content.find("ScriptLoader") != std::string::npos) {
        return {FragmentType::CHTL_JS, content};
    }
    return {FragmentType::JS, content};
}

ScannedOutput UnifiedScanner::scan(const std::string& source) {
    this->output = ScannedOutput();
    this->placeholder_id_counter = 0;
    std::string mutable_source = source;

    this->output.chtl_with_placeholders = recursive_scan_and_replace(mutable_source, FragmentType::CHTL);

    return this->output;
}

std::string UnifiedScanner::recursive_scan_and_replace(std::string& content, FragmentType parent_type) {
    std::string processed_content;
    size_t last_pos = 0;

    for (size_t i = 0; i < content.length(); ++i) {
        bool is_style = is_keyword_at(content, i, "style");
        bool is_script = is_keyword_at(content, i, "script");

        // The top-level CHTL scan looks for style and script blocks.
        if (parent_type == FragmentType::CHTL && (is_style || is_script)) {
            size_t block_start = content.find('{', i);
            if (block_start == std::string::npos) continue;

            size_t block_end = find_matching_brace(content, block_start);
            if (block_end == std::string::npos) continue;

            processed_content.append(content, last_pos, i - last_pos);

            std::string block_content = content.substr(block_start + 1, block_end - block_start - 1);

            if (is_style) {
                // Style blocks are treated as CSS for recursive scanning.
                std::string processed_block_content = recursive_scan_and_replace(block_content, FragmentType::CSS);
                processed_content += "style {" + processed_block_content + "}";
            } else { // is_script
                // Script blocks are treated as CHTL_JS for recursive scanning.
                std::string processed_block_content = recursive_scan_and_replace(block_content, FragmentType::CHTL_JS);
                processed_content += "script {" + processed_block_content + "}";
            }

            last_pos = block_end + 1;
            i = block_end;
        }
        // Inside a CHTL_JS block, look for standard JS function blocks to extract.
        else if (parent_type == FragmentType::CHTL_JS && is_keyword_at(content, i, "function")) {
            size_t block_start = content.find('{', i);
            if (block_start == std::string::npos) continue;

            size_t block_end = find_matching_brace(content, block_start);
            if (block_end == std::string::npos) continue;

            std::string function_full = content.substr(i, block_end - i + 1);

            // Create a placeholder for the entire JS function.
            std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "__";
            output.fragments[placeholder] = {FragmentType::JS, function_full};

            processed_content.append(content, last_pos, i - last_pos);
            processed_content.append(placeholder);

            last_pos = block_end + 1;
            i = block_end;
        }
        // Inside a CSS block, look for CHTL features to extract.
        else if (parent_type == FragmentType::CSS) {
             // This regex finds CHTL features inside a style block.
             std::regex chtl_in_css_regex(R"((@Style\s+[a-zA-Z0-9_]+;)|([\w-]+\s*:\s*[^;]*[+\-*/][^;]*;))");
             std::smatch match;
             if (std::regex_search(content.cbegin() + i, content.cend(), match, chtl_in_css_regex)) {
                 if (match.position() == 0) { // Ensure the match is at the current position
                    std::string chtl_feature = match.str();
                    std::string placeholder = "/*_CHTL_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "_*/";
                    output.fragments[placeholder] = {FragmentType::CHTL, chtl_feature};

                    processed_content.append(content, last_pos, i - last_pos);
                    processed_content.append(placeholder);

                    last_pos = i + chtl_feature.length();
                    i = last_pos -1;
                 }
             }
        }
    }

    processed_content.append(content, last_pos, content.length() - last_pos);
    return processed_content;
}