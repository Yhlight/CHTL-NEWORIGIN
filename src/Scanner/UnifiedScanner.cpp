#include "UnifiedScanner.h"
#include <string>
#include <cctype>
#include <regex>

// Helper to check if a substring at a position is a keyword, ensuring it's a whole word.
bool is_keyword_at(const std::string& s, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > s.length()) {
        return false;
    }
    if (s.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    // Check for word boundary before the keyword
    if (pos > 0 && (isalnum(s[pos - 1]) || s[pos - 1] == '_')) {
        return false;
    }
    // Check for word boundary after the keyword
    if (pos + keyword.length() < s.length() && (isalnum(s[pos + keyword.length()]) || s[pos + keyword.length()] == '_')) {
        return false;
    }
    return true;
}

// New, more robust method to find a matching closing brace
size_t UnifiedScanner::find_matching_brace(const std::string& s, size_t start_pos) {
    if (start_pos >= s.length() || s[start_pos] != '{') {
        return std::string::npos;
    }

    int brace_level = 1;
    ParseState state = ParseState::NORMAL;
    size_t i = start_pos + 1;

    while (i < s.length()) {
        char current = s[i];
        char prev = (i > 0) ? s[i - 1] : '\0';

        switch (state) {
            case ParseState::NORMAL:
                if (current == '{') {
                    brace_level++;
                } else if (current == '}') {
                    brace_level--;
                    if (brace_level == 0) {
                        return i;
                    }
                } else if (current == '/' && i + 1 < s.length() && s[i + 1] == '/') {
                    state = ParseState::IN_LINE_COMMENT;
                    i++;
                } else if (current == '/' && i + 1 < s.length() && s[i + 1] == '*') {
                    state = ParseState::IN_BLOCK_COMMENT;
                    i++;
                } else if (current == '\'') {
                    state = ParseState::IN_SINGLE_QUOTE_STRING;
                } else if (current == '"') {
                    state = ParseState::IN_DOUBLE_QUOTE_STRING;
                } else if (current == '`') {
                    state = ParseState::IN_TEMPLATE_LITERAL;
                }
                break;

            case ParseState::IN_SINGLE_QUOTE_STRING:
                if (current == '\'' && prev != '\\') state = ParseState::NORMAL;
                break;
            case ParseState::IN_DOUBLE_QUOTE_STRING:
                if (current == '"' && prev != '\\') state = ParseState::NORMAL;
                break;
            case ParseState::IN_TEMPLATE_LITERAL:
                if (current == '`' && prev != '\\') state = ParseState::NORMAL;
                break;
            case ParseState::IN_LINE_COMMENT:
                if (current == '\n') state = ParseState::NORMAL;
                break;
            case ParseState::IN_BLOCK_COMMENT:
                if (current == '/' && prev == '*') state = ParseState::NORMAL;
                break;
        }
        i++;
    }

    return std::string::npos; // No matching brace found
}


// Heuristic to differentiate CHTL JS from plain JS.
CodeFragment UnifiedScanner::processScriptBlock(const std::string& content) {
    // Look for CHTL JS-specific syntax.
    if (content.find("{{") != std::string::npos ||
        content.find("}}") != std::string::npos ||
        content.find("->") != std::string::npos ||
        is_keyword_at(content, 0, "Listen") ||
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
    std::string placeholder_template = "CHTL_PLACEHOLDER_";
    std::string result_source;
    size_t last_pos = 0;

    for (size_t i = 0; i < source.length(); ++i) {
        bool is_style = is_keyword_at(source, i, "style");
        bool is_script = is_keyword_at(source, i, "script");
        bool is_listen = is_keyword_at(source, i, "Listen");

        if (is_style || is_script || is_listen) {
            size_t keyword_len = is_listen ? 6 : (is_style ? 5 : 6);
            size_t block_start_pos = i + keyword_len;

            while (block_start_pos < source.length() && isspace(source[block_start_pos])) {
                block_start_pos++;
            }

            if (block_start_pos < source.length() && source[block_start_pos] == '{') {
                size_t block_end_pos = find_matching_brace(source, block_start_pos);

                if (block_end_pos != std::string::npos) {
                    result_source.append(source, last_pos, i - last_pos);

                    std::string content = source.substr(block_start_pos + 1, block_end_pos - block_start_pos - 1);

                    if (is_style) {
                        // For style blocks, parse the content for CHTL features.
                        std::string processed_content;
                        auto current_pos_in_content = content.cbegin();

                        std::regex chtl_in_css_regex(
                            // Group 1: @Style MyTemplate;
                            R"((@Style\s+[a-zA-Z0-9_]+;))"
                            "|"
                            // Group 2: Full property, Group 3: name, Group 4: value with arithmetic
                            R"((([\w-]+)\s*:\s*([^;]*[+\-*/][^;]*));)"
                        );

                        auto it_begin = std::sregex_iterator(content.begin(), content.end(), chtl_in_css_regex);
                        auto it_end = std::sregex_iterator();

                        for (std::sregex_iterator it = it_begin; it != it_end; ++it) {
                            const std::smatch& match = *it;
                            processed_content += match.prefix();
                            std::string placeholder = placeholder_template + std::to_string(placeholder_id_counter++);

                            if (match[1].matched) { // Matched @Style usage
                                output.fragments[placeholder] = {FragmentType::CHTL, match[1].str()};
                                processed_content += placeholder;
                            } else if (match[2].matched) { // Matched arithmetic property
                                std::string property_name = match[3].str();
                                std::string value = match[4].str();

                                // Trim leading/trailing whitespace from the captured value
                                value.erase(0, value.find_first_not_of(" \t\n\r"));
                                value.erase(value.find_last_not_of(" \t\n\r") + 1);

                                output.fragments[placeholder] = {FragmentType::CHTL, value};
                                processed_content += property_name + ": " + placeholder + ";";
                            }
                            current_pos_in_content = match.suffix().first;
                        }
                        processed_content += std::string(current_pos_in_content, content.cend());
                        result_source += "style {" + processed_content + "}";

                    } else {
                        // For script/listen blocks, extract the whole block.
                        std::string placeholder = placeholder_template + std::to_string(placeholder_id_counter++);
                        if (is_script) {
                            output.fragments[placeholder] = processScriptBlock(content);
                        } else { // is_listen
                            std::string listen_content = source.substr(i, block_end_pos - i + 1);
                            output.fragments[placeholder] = {FragmentType::CHTL_JS, listen_content};
                        }

                        if (is_listen) {
                            result_source += placeholder;
                        } else {
                            result_source += source.substr(i, keyword_len);
                            result_source += " " + placeholder + "{}";
                        }
                    }

                    last_pos = block_end_pos + 1;
                    i = block_end_pos;
                }
            }
        }
    }

    if (last_pos < source.length()) {
        result_source.append(source, last_pos, source.length() - last_pos);
    }

    output.chtl_with_placeholders = result_source;
    return output;
}