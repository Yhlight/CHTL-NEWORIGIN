#include "UnifiedScanner.h"
#include "Util/StringUtils.h"
#include <regex>
#include <iostream>
#include <algorithm>
#include <cctype>

std::string UnifiedScanner::scan(const std::string& source) {
    std::string result = "";
    size_t current_pos = 0;

    // `Vir` is separated from the lookahead group so it's matched as a standalone keyword.
    std::regex chtljs_regex(
        R"(\{\{|\b(ScriptLoader|Listen|Delegate|Animate|Router|iNeverAway|util)\b(?=\s*\{)|\bVir\b|&->|->)"
    );
    std::smatch match;

    while (current_pos < source.length() && std::regex_search(source.cbegin() + current_pos, source.cend(), match, chtljs_regex)) {
        size_t match_pos = current_pos + match.position(0);

        if (match_pos > current_pos) {
            result += addPlaceholder(source.substr(current_pos, match_pos - current_pos));
        }

        std::string match_str = match.str(0);
        size_t consumed_length = 0;

        if (match_str == "{{") {
            size_t end_marker = source.find("}}", match_pos);
            consumed_length = (end_marker != std::string::npos) ? (end_marker - match_pos + 2) : match_str.length();
        } else if (match_str == "->" || match_str == "&->") {
            consumed_length = match_str.length();
        } else { // Keyword like Listen, Animate
            size_t open_brace = source.find('{', match_pos + match_str.length());
            if (open_brace != std::string::npos) {
                size_t close_brace = findMatchingBrace(source, open_brace);
                if (close_brace != std::string::npos) {
                    size_t end_of_statement = close_brace + 1;
                    if (end_of_statement < source.length() && source[end_of_statement] == ';') {
                        end_of_statement++;
                    }
                    consumed_length = end_of_statement - match_pos;
                }
            }
        }

        if (consumed_length == 0) {
            consumed_length = match_str.length(); // Fallback
        }

        result += source.substr(match_pos, consumed_length);
        current_pos = match_pos + consumed_length;
    }

    if (current_pos < source.length()) {
        result += addPlaceholder(source.substr(current_pos));
    }

    return result;
}

const std::map<std::string, std::string>& UnifiedScanner::getPlaceholders() const {
    return placeholder_map;
}

std::string UnifiedScanner::addPlaceholder(const std::string& code) {
    // Use a lambda with an explicit unsigned char cast to safely call std::isspace.
    if (std::all_of(code.begin(), code.end(), [](unsigned char c){ return std::isspace(c); })) {
        return code;
    }

    if (code.empty()) {
        return "";
    }

    std::string placeholder = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_count++) + "_";
    placeholder_map[placeholder] = code;
    return placeholder;
}