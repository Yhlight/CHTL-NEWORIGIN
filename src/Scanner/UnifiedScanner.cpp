#include "UnifiedScanner.h"
#include <string>
#include <cctype>
#include <regex>
#include <vector>
#include <algorithm>

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
    std::string mutable_content = content;
    std::string result = recursive_scan_and_replace(mutable_content, FragmentType::CHTL_JS);
    return {FragmentType::CHTL_JS, result};
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

    if (parent_type == FragmentType::CHTL) {
        for (size_t i = 0; i < content.length(); ++i) {
            if (is_keyword_at(content, i, "style") || is_keyword_at(content, i, "script")) {
                size_t block_start = content.find('{', i);
                if (block_start == std::string::npos) continue;
                size_t block_end = find_matching_brace(content, block_start);
                if (block_end == std::string::npos) continue;
                processed_content.append(content, last_pos, i - last_pos);
                std::string block_content = content.substr(block_start + 1, block_end - block_start - 1);
                bool is_style = is_keyword_at(content, i, "style");
                std::string processed_block = recursive_scan_and_replace(block_content, is_style ? FragmentType::CSS : FragmentType::CHTL_JS);
                processed_content += (is_style ? "style" : "script");
                processed_content += " {" + processed_block + "}";
                last_pos = block_end + 1;
                i = block_end;
            }
        }
    } else if (parent_type == FragmentType::CHTL_JS) {
        // Inside a script block, separate CHTL_JS from standard JS
        std::vector<std::string> chtl_js_keywords = {"Listen", "Delegate", "Animate", "Router", "Vir", "ScriptLoader"};
        for (size_t i = 0; i < content.length(); ++i) {
            bool is_chtl_js = false;
            size_t construct_end = i;
            for (const auto& keyword : chtl_js_keywords) {
                if (is_keyword_at(content, i, keyword)) {
                    size_t block_start = content.find('{', i);
                    if (block_start != std::string::npos) {
                        size_t block_end = find_matching_brace(content, block_start);
                        if (block_end != std::string::npos) { is_chtl_js = true; construct_end = block_end; }
                    }
                    break;
                }
            }
            if (!is_chtl_js && content.substr(i, 2) == "{{") {
                size_t end_brace = content.find("}}", i);
                if (end_brace != std::string::npos) { is_chtl_js = true; construct_end = end_brace + 1; }
            }
            if (is_chtl_js) {
                if (i > last_pos) {
                    std::string js_fragment = content.substr(last_pos, i - last_pos);
                    if (!std::all_of(js_fragment.begin(), js_fragment.end(), isspace)) {
                        std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "__";
                        output.fragments[placeholder] = {FragmentType::JS, js_fragment};
                        processed_content.append(placeholder);
                    }
                }
                processed_content.append(content, i, construct_end - i + 1);
                last_pos = construct_end + 1;
                i = construct_end;
            }
        }
    } else if (parent_type == FragmentType::CSS) {
        // Inside a style block, extract standard CSS rules, leaving CHTL features behind.
        for (size_t i = 0; i < content.length(); ++i) {
            if (isspace(content[i])) continue;
            // CHTL features in CSS start with an identifier or '@'.
            // Standard CSS rules start with other symbols (., #, *, [) or an identifier.
            if (content[i] != '@' && (content[i] == '.' || content[i] == '#' || content[i] == '*' || content[i] == '[' || isalnum(content[i]))) {
                size_t block_start = content.find('{', i);
                if (block_start != std::string::npos) {
                    // Check if there is a semicolon before the brace, which would indicate a CHTL expression, not a CSS rule
                    size_t semi_pos = content.find(';', i);
                    if (semi_pos != std::string::npos && semi_pos < block_start) {
                        continue; // It's a CHTL expression, not a CSS rule.
                    }
                    size_t block_end = find_matching_brace(content, block_start);
                    if (block_end != std::string::npos) {
                        processed_content.append(content, last_pos, i - last_pos);
                        std::string css_rule = content.substr(i, block_end - i + 1);
                        std::string placeholder = "/*__CSS_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "__*/";
                        output.fragments[placeholder] = {FragmentType::CSS, css_rule};
                        processed_content.append(placeholder);
                        last_pos = block_end + 1;
                        i = block_end;
                    }
                }
            }
        }
    }

    if (last_pos < content.length()) {
        std::string remaining_fragment = content.substr(last_pos);
        if (!std::all_of(remaining_fragment.begin(), remaining_fragment.end(), isspace)) {
             if (parent_type == FragmentType::CHTL_JS) {
                std::string placeholder = "__JS_PLACEHOLDER_" + std::to_string(placeholder_id_counter++) + "__";
                output.fragments[placeholder] = {FragmentType::JS, remaining_fragment};
                processed_content.append(placeholder);
            } else {
                processed_content.append(remaining_fragment);
            }
        }
    }
    return processed_content;
}