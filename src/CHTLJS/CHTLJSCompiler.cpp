#include "CHTLJSCompiler.h"
#include "Util/StringUtils.h"
#include <regex>
#include <iostream>
#include <string>

// A simplified helper to convert a CHTL JS selector to a JS querySelector
std::string convertSelector(const std::string& chtl_selector) {
    if (chtl_selector.empty()) {
        return "";
    }
    // Simple case: .class, #id
    if (chtl_selector[0] == '.' || chtl_selector[0] == '#') {
        return "document.querySelector('" + chtl_selector + "')";
    }
    // Simple case: tag (this is ambiguous, could be querySelectorAll, but we'll use querySelector for now)
    return "document.querySelector('" + chtl_selector + "')";
}

std::string CHTLJSCompiler::compileSelectors(std::string source) {
    std::regex selector_regex(R"(\{\{([^}]+)\}\})");
    auto it = std::sregex_iterator(source.begin(), source.end(), selector_regex);
    auto end = std::sregex_iterator();

    if (it == end) {
        return source; // No selectors found, return original string
    }

    std::string result;
    size_t last_pos = 0;

    for (; it != end; ++it) {
        std::smatch match = *it;
        result.append(source, last_pos, match.position() - last_pos);
        std::string inner_selector = match[1].str();
        result.append(convertSelector(inner_selector));
        last_pos = match.position() + match.length();
    }

    result.append(source, last_pos, std::string::npos);
    return result;
}

std::string CHTLJSCompiler::compileListenFunctions(std::string source) {
    std::string result;
    size_t last_pos = 0;

    // Regex to find the start of a Listen expression
    std::regex listen_start_regex(R"((\{\{[^}]+\}\}|[\w\d_.]+)\s*->\s*Listen)");

    auto it = std::sregex_iterator(source.begin(), source.end(), listen_start_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        result += source.substr(last_pos, match.position() - last_pos);

        std::string selector_str = match[1].str();

        // Find the opening brace that follows the "Listen" keyword
        size_t open_brace_pos = source.find('{', match.position() + match.length());
        if (open_brace_pos == std::string::npos) continue;

        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) continue;

        std::string events_str = source.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);

        // Compile the selector part
        std::string compiled_selector = selector_str;
        if (selector_str.rfind("{{", 0) == 0) {
            std::string inner_selector = selector_str.substr(2, selector_str.length() - 4);
            compiled_selector = convertSelector(inner_selector);
        }

        // --- Manual Parsing of Events ---
        size_t current_pos = 0;
        while (current_pos < events_str.length()) {
            size_t colon_pos = events_str.find(':', current_pos);
            if (colon_pos == std::string::npos) break;

            std::string event_name = events_str.substr(current_pos, colon_pos - current_pos);
            event_name.erase(0, event_name.find_first_not_of(" \t\n\r"));
            event_name.erase(event_name.find_last_not_of(" \t\n\r") + 1);
            if (event_name.empty()) {
                current_pos = colon_pos + 1;
                continue;
            }

            size_t value_start = events_str.find_first_not_of(" \t\n\r", colon_pos + 1);
            if (value_start == std::string::npos) break;

            size_t value_end = value_start;
            int brace_balance = 0;
            for (; value_end < events_str.length(); ++value_end) {
                char c = events_str[value_end];
                if (c == '{') brace_balance++;
                else if (c == '}') brace_balance--;
                else if (c == ',' && brace_balance == 0) break;
            }

            std::string handler = events_str.substr(value_start, value_end - value_start);
            handler.erase(handler.find_last_not_of(" \t\n\r") + 1);

            if (!handler.empty()) {
                 result += compiled_selector + ".addEventListener('" + event_name + "', " + handler + ");\n";
            }

            current_pos = value_end + 1;
        }
        // --- End Manual Parsing ---

        last_pos = close_brace_pos + 1;
    }

    result += source.substr(last_pos);

    return result;
}


std::string CHTLJSCompiler::compile(const std::string& scanned_source) {
    std::string result = scanned_source;
    result = compileListenFunctions(result); // Compile Listen first as it contains selectors
    result = compileSelectors(result);       // Compile any standalone selectors
    return result;
}