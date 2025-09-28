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


// This is a simplified parser for the content inside an Animate block.
// A real implementation would need a more robust parser.
std::string parseAnimateBlock(const std::string& block_content) {
    std::string js_code = "(function() {\n";
    std::string target, duration, begin_styles, end_styles;

    // More robust, non-greedy regex that also handles CHTL JS selectors.
    std::regex prop_regex(R"((\w+)\s*:\s*(\{\{.*?\}\}|\{.*?\}|\[.*?\]|[\w\d_.-]+))");
    auto it = std::sregex_iterator(block_content.begin(), block_content.end(), prop_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        std::string key = match[1].str();
        std::string value = match[2].str();

        if (key == "target") {
            target = value;
        } else if (key == "duration") {
            duration = value;
        } else if (key == "begin") {
            begin_styles = value;
        } else if (key == "end") {
            end_styles = value;
        }
    }

    if (target.empty() || duration.empty()) {
        return ""; // Not enough info to create animation
    }

    // Compile selector if needed
    if (target.rfind("{{", 0) == 0) {
        std::string inner_selector = target.substr(2, target.length() - 4);
        target = convertSelector(inner_selector);
    }

    js_code += "const target_elem = " + target + ";\n";
    js_code += "if (!target_elem) return;\n";

    // Apply begin styles
    if (!begin_styles.empty()) {
        begin_styles = std::regex_replace(begin_styles, std::regex("[{}]"), "");
        std::regex style_regex(R"((\w+)\s*:\s*'([^']*)')");
        auto style_it = std::sregex_iterator(begin_styles.begin(), begin_styles.end(), style_regex);
        for (; style_it != end; ++style_it) {
            std::smatch style_match = *style_it;
            js_code += "target_elem.style." + style_match[1].str() + " = '" + style_match[2].str() + "';\n";
        }
    }

    // Apply end styles after duration
    if (!end_styles.empty()) {
        end_styles = std::regex_replace(end_styles, std::regex("[{}]"), "");
        std::string end_style_code;
        std::regex style_regex(R"((\w+)\s*:\s*'([^']*)')");
        auto style_it = std::sregex_iterator(end_styles.begin(), end_styles.end(), style_regex);
        for (; style_it != end; ++style_it) {
            std::smatch style_match = *style_it;
            end_style_code += "target_elem.style." + style_match[1].str() + " = '" + style_match[2].str() + "';\n";
        }
        js_code += "setTimeout(() => {\n" + end_style_code + "}, " + duration + ");\n";
    }

    js_code += "})();\n";
    return js_code;
}


std::string CHTLJSCompiler::compileAnimateFunctions(std::string source) {
    std::string result;
    size_t last_pos = 0;

    // The leading \b is removed to allow matching after a placeholder.
    std::regex animate_start_regex(R"(Animate\b(?=\s*\{))");

    auto it = std::sregex_iterator(source.begin(), source.end(), animate_start_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        result += source.substr(last_pos, match.position() - last_pos);

        size_t open_brace_pos = source.find('{', match.position() + match.length());
        if (open_brace_pos == std::string::npos) continue;

        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) continue;

        std::string block_content = source.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);

        result += parseAnimateBlock(block_content);

        last_pos = close_brace_pos + 1;
        // Consume semicolon
        if (last_pos < source.length() && source[last_pos] == ';') {
            last_pos++;
        }
    }

    result += source.substr(last_pos);

    return result;
}


std::string parseDelegateBlock(const std::string& parent_selector, const std::string& block_content) {
    std::string js_code;
    std::string target_selector;
    std::map<std::string, std::string> event_handlers;

    // Regex to find properties like target: '.child' or click: handleClick
    std::regex prop_regex(R"((\w+)\s*:\s*('[\w\d_.-]*'|[\w\d_.-]+))");
    auto it = std::sregex_iterator(block_content.begin(), block_content.end(), prop_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        std::string key = match[1].str();
        std::string value = match[2].str();

        if (key == "target") {
            // Remove quotes if they exist
            if (value.front() == '\'' && value.back() == '\'') {
                target_selector = value.substr(1, value.length() - 2);
            } else {
                target_selector = value;
            }
        } else {
            event_handlers[key] = value;
        }
    }

    if (parent_selector.empty() || target_selector.empty() || event_handlers.empty()) {
        return ""; // Not enough info to create delegation
    }

    std::string compiled_parent = parent_selector;
    if (parent_selector.rfind("{{", 0) == 0) {
        std::string inner = parent_selector.substr(2, parent_selector.length() - 4);
        compiled_parent = convertSelector(inner);
    }

    js_code += "const parent_elem = " + compiled_parent + ";\n";
    js_code += "if (parent_elem) {\n";

    for (const auto& pair : event_handlers) {
        const std::string& event_name = pair.first;
        const std::string& handler_name = pair.second;

        js_code += "  parent_elem.addEventListener('" + event_name + "', (event) => {\n";
        js_code += "    if (event.target.matches('" + target_selector + "')) {\n";
        js_code += "      " + handler_name + "(event);\n";
        js_code += "    }\n";
        js_code += "  });\n";
    }

    js_code += "}\n";
    return js_code;
}

std::string CHTLJSCompiler::compileDelegateFunctions(std::string source) {
    std::string result;
    size_t last_pos = 0;

    std::regex delegate_start_regex(R"((\{\{[^}]+\}\}|[\w\d_.]+)\s*->\s*Delegate)");

    auto it = std::sregex_iterator(source.begin(), source.end(), delegate_start_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        result += source.substr(last_pos, match.position() - last_pos);

        std::string parent_selector = match[1].str();

        size_t open_brace_pos = source.find('{', match.position() + match.length());
        if (open_brace_pos == std::string::npos) continue;

        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) continue;

        std::string block_content = source.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);

        result += parseDelegateBlock(parent_selector, block_content);

        last_pos = close_brace_pos + 1;
        if (last_pos < source.length() && source[last_pos] == ';') {
            last_pos++;
        }
    }

    result += source.substr(last_pos);
    return result;
}

// Manual parser for the Router block, avoiding complex regex issues.
std::string parseRouterBlock(const std::string& block_content) {
    std::string js_code;
    std::map<std::string, std::string> routes;
    std::string mode = "hash"; // default mode
    std::string current_url;

    size_t pos = 0;
    while (pos < block_content.length()) {
        size_t key_start = block_content.find_first_not_of(" \t\n\r,", pos);
        if (key_start == std::string::npos) break;

        size_t colon_pos = block_content.find(':', key_start);
        if (colon_pos == std::string::npos) break;

        std::string key = block_content.substr(key_start, colon_pos - key_start);
        key.erase(key.find_last_not_of(" \t\n\r") + 1);

        size_t value_start = block_content.find_first_not_of(" \t\n\r", colon_pos + 1);
        if (value_start == std::string::npos) break;

        size_t value_end;
        char first_char = block_content[value_start];
        if (first_char == '\'' || first_char == '"') {
            value_end = block_content.find(first_char, value_start + 1);
            if (value_end != std::string::npos) {
                value_end++; // include the closing quote
            }
        } else if (block_content.substr(value_start, 2) == "{{") {
            value_end = block_content.find("}}", value_start);
             if (value_end != std::string::npos) {
                value_end += 2; // include the closing braces
            }
        } else {
            value_end = block_content.find_first_of(", \t\n\r", value_start);
        }

        if (value_end == std::string::npos) {
            value_end = block_content.length();
        }

        std::string value = block_content.substr(value_start, value_end - value_start);
        value.erase(value.find_last_not_of(" \t\n\r,") + 1);

        // Trim quotes from value if they exist
        if ((value.front() == '\'' && value.back() == '\'') || (value.front() == '"' && value.back() == '"')) {
            value = value.substr(1, value.length() - 2);
        }

        if (key == "url") {
            current_url = value;
        } else if (key == "page" && !current_url.empty()) {
            routes[current_url] = value;
            current_url.clear();
        } else if (key == "mode") {
            mode = value;
        }

        pos = value_end;
    }

    if (routes.empty() || mode != "hash") {
        return "// Router not configured or mode not supported yet.\n";
    }

    js_code += "const routes = {\n";
    for(const auto& route : routes) {
        std::string page_selector = route.second;
        if (page_selector.rfind("{{", 0) == 0) {
            page_selector = convertSelector(page_selector.substr(2, page_selector.length() - 4));
        }
        js_code += "  '" + route.first + "': " + page_selector + ",\n";
    }
    js_code += "};\n\n";

    js_code += R"JS(
function navigate() {
    const path = window.location.hash.slice(1) || '/';
    const page = routes[path];

    // Hide all pages
    for (const key in routes) {
        if (routes[key]) routes[key].style.display = 'none';
    }

    // Show the current page
    if (page) {
        page.style.display = 'block';
    }
}

window.addEventListener('hashchange', navigate);
navigate(); // Initial navigation
)JS";

    return js_code;
}

std::string CHTLJSCompiler::compileRouterFunctions(std::string source) {
    std::string result;
    size_t last_pos = 0;

    std::regex router_start_regex(R"(Router\b(?=\s*\{))");

    auto it = std::sregex_iterator(source.begin(), source.end(), router_start_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        result += source.substr(last_pos, match.position() - last_pos);

        size_t open_brace_pos = source.find('{', match.position() + match.length());
        if (open_brace_pos == std::string::npos) continue;

        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) continue;

        std::string block_content = source.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);

        result += parseRouterBlock(block_content);

        last_pos = close_brace_pos + 1;
        if (last_pos < source.length() && source[last_pos] == ';') {
            last_pos++;
        }
    }

    result += source.substr(last_pos);
    return result;
}


std::string CHTLJSCompiler::compile(const std::string& scanned_source) {
    std::string result = scanned_source;
    result = compileListenFunctions(result);
    result = compileAnimateFunctions(result);
    result = compileDelegateFunctions(result);
    result = compileRouterFunctions(result);
    result = compileSelectors(result);
    return result;
}