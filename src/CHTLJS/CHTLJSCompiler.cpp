#include "CHTLJSCompiler.h"
#include "Util/StringUtils.h"
#include <regex>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

// A simplified helper to convert a CHTL JS selector to a JS querySelector
std::string convertSelector(const std::string& chtl_selector) {
    if (chtl_selector.empty()) {
        return "";
    }
    if (chtl_selector[0] == '.' || chtl_selector[0] == '#') {
        return "document.querySelector('" + chtl_selector + "')";
    }
    return "document.querySelector('" + chtl_selector + "')";
}

// Helper to parse a CHTL JS keyframe block like "{ at: 0.5, ... }" into a JS object string.
std::string parseKeyframeObject(const std::string& block) {
    std::string js_object = "{ ";
    std::string content = block.substr(1, block.length() - 2); // remove braces

    std::regex prop_regex(R"(([\w-]+)\s*:\s*([\d.]+|'[^']+'))");
    auto it = std::sregex_iterator(content.begin(), content.end(), prop_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        std::string key = match[1].str();
        std::string value = match[2].str();

        if (key == "at") {
            key = "offset";
        } else {
            size_t hyphen_pos = key.find('-');
            while (hyphen_pos != std::string::npos) {
                key.erase(hyphen_pos, 1);
                if (hyphen_pos < key.length()) {
                    key[hyphen_pos] = toupper(key[hyphen_pos]);
                }
                hyphen_pos = key.find('-');
            }
        }
        js_object += key + ": " + value + ", ";
    }

    if (js_object.length() > 2) {
        js_object.pop_back();
        js_object.pop_back();
    }

    js_object += " }";
    return js_object;
}

std::string CHTLJSCompiler::compileSelectors(std::string source) {
    std::regex selector_regex(R"(\{\{([^}]+)\}\})");
    auto it = std::sregex_iterator(source.begin(), source.end(), selector_regex);
    auto end = std::sregex_iterator();

    if (it == end) {
        return source;
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
    std::regex listen_start_regex(R"((\{\{[^}]+\}\}|[\w\d_.]+)\s*->\s*Listen)");
    auto it = std::sregex_iterator(source.begin(), source.end(), listen_start_regex);
    auto end = std::sregex_iterator();
    for (; it != end; ++it) {
        std::smatch match = *it;
        result += source.substr(last_pos, match.position() - last_pos);
        std::string selector_str = match[1].str();
        size_t open_brace_pos = source.find('{', match.position() + match.length());
        if (open_brace_pos == std::string::npos) continue;
        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) continue;
        std::string events_str = source.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);
        std::string compiled_selector = selector_str;
        if (selector_str.rfind("{{", 0) == 0) {
            std::string inner_selector = selector_str.substr(2, selector_str.length() - 4);
            compiled_selector = convertSelector(inner_selector);
        }
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
        last_pos = close_brace_pos + 1;
    }
    result += source.substr(last_pos);
    return result;
}

std::string parseAnimateBlock(const std::string& block_content);
std::string CHTLJSCompiler::compileAnimateFunctions(std::string source) {
    std::string result;
    size_t last_pos = 0;
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
        if (last_pos < source.length() && source[last_pos] == ';') {
            last_pos++;
        }
    }
    result += source.substr(last_pos);
    return result;
}

std::string parseDelegateBlock(const std::string& parent_selector, const std::string& block_content);
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

std::string parseRouterBlock(const std::string& block_content);
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

std::string parseScriptLoaderBlock(const std::string& block_content);
std::string CHTLJSCompiler::compileScriptLoaderFunctions(std::string source) {
    std::string result;
    size_t last_pos = 0;
    std::regex loader_start_regex(R"(ScriptLoader\b(?=\s*\{))");
    auto it = std::sregex_iterator(source.begin(), source.end(), loader_start_regex);
    auto end = std::sregex_iterator();
    for (; it != end; ++it) {
        std::smatch match = *it;
        result += source.substr(last_pos, match.position() - last_pos);
        size_t open_brace_pos = source.find('{', match.position() + match.length());
        if (open_brace_pos == std::string::npos) continue;
        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) continue;
        std::string block_content = source.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);
        result += parseScriptLoaderBlock(block_content);
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
    result = compileScriptLoaderFunctions(result);
    result = compileListenFunctions(result);
    result = compileAnimateFunctions(result);
    result = compileDelegateFunctions(result);
    result = compileRouterFunctions(result);
    result = compileSelectors(result);
    return result;
}

// --- PARSER IMPLEMENTATIONS WITH ERROR HANDLING ---

std::string parseAnimateBlock(const std::string& block_content) {
    std::string js_code;
    std::string target, duration, callback;
    std::vector<std::string> keyframes_js;
    std::string options_js = "{ ";

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
        if (first_char == '{') value_end = findMatchingBrace(block_content, value_start) + 1;
        else if (first_char == '[') {
            int balance = 1;
            value_end = value_start + 1;
            while(value_end < block_content.length() && balance > 0) {
                if(block_content[value_end] == '[') balance++;
                if(block_content[value_end] == ']') balance--;
                value_end++;
            }
        } else {
            value_end = block_content.find_first_of(",\n", value_start);
            if (value_end == std::string::npos) value_end = block_content.length();
        }
        std::string value = block_content.substr(value_start, value_end - value_start);
        value.erase(value.find_last_not_of(" \t\n\r,") + 1);
        std::string raw_value = value;
        if ((value.front() == '\'' && value.back() == '\'') || (value.front() == '"' && value.back() == '"')) {
            raw_value = value.substr(1, value.length() - 2);
        }
        if (key == "target") target = value;
        else if (key == "duration") {
            duration = raw_value;
            options_js += key + ": " + raw_value + ", ";
        }
        else if (key == "delay") options_js += key + ": " + raw_value + ", ";
        else if (key == "easing" || key == "direction") options_js += key + ": '" + raw_value + "', ";
        else if (key == "loop") {
            if (raw_value == "-1") options_js += "iterations: Infinity, ";
            else options_js += "iterations: " + raw_value + ", ";
        }
        else if (key == "callback") callback = raw_value;
        else if (key == "begin") keyframes_js.insert(keyframes_js.begin(), parseKeyframeObject(value));
        else if (key == "end") keyframes_js.push_back(parseKeyframeObject(value));
        else if (key == "when") {
            std::string when_content = value.substr(1, value.length() - 2);
            size_t when_pos = 0;
            while(when_pos < when_content.length()) {
                size_t frame_start = when_content.find('{', when_pos);
                if (frame_start == std::string::npos) break;
                size_t frame_end = findMatchingBrace(when_content, frame_start);
                if (frame_end == std::string::npos) break;
                keyframes_js.push_back(parseKeyframeObject(when_content.substr(frame_start, frame_end - frame_start + 1)));
                when_pos = frame_end + 1;
            }
        }
        pos = value_end;
    }

    if (target.empty()) return "console.error(\"CHTL JS Animate Error: 'target' property is required.\");\n";
    if (duration.empty()) return "console.error(\"CHTL JS Animate Error: 'duration' property is required.\");\n";

    if (options_js.length() > 2) {
        options_js.pop_back();
        options_js.pop_back();
    }
    options_js += " }";
    if (target.rfind("{{", 0) == 0) {
        target = convertSelector(target.substr(2, target.length() - 4));
    }
    js_code += "const target_elem = " + target + ";\n";
    js_code += "if (target_elem) {\n";
    js_code += "  const keyframes = [ ";
    for(size_t i = 0; i < keyframes_js.size(); ++i) {
        js_code += keyframes_js[i] + (i == keyframes_js.size() - 1 ? "" : ", ");
    }
    js_code += " ];\n";
    js_code += "  const animation = target_elem.animate(keyframes, " + options_js + ");\n";
    if (!callback.empty()) {
        js_code += "  animation.finished.then(" + callback + ");\n";
    }
    js_code += "}\n";
    return js_code;
}

std::string parseDelegateBlock(const std::string& parent_selector, const std::string& block_content) {
    std::string js_code;
    std::string target_selector;
    std::map<std::string, std::string> event_handlers;
    std::regex prop_regex(R"((\w+)\s*:\s*('[\w\d_.-]*'|[\w\d_.-]+))");
    auto it = std::sregex_iterator(block_content.begin(), block_content.end(), prop_regex);
    auto end = std::sregex_iterator();
    for (; it != end; ++it) {
        std::smatch match = *it;
        std::string key = match[1].str();
        std::string value = match[2].str();
        if (key == "target") {
            if (value.front() == '\'' && value.back() == '\'') {
                target_selector = value.substr(1, value.length() - 2);
            } else {
                target_selector = value;
            }
        } else {
            event_handlers[key] = value;
        }
    }

    if (parent_selector.empty()) return "console.error(\"CHTL JS Delegate Error: A parent selector is required.\");\n";
    if (target_selector.empty()) return "console.error(\"CHTL JS Delegate Error: 'target' property is required.\");\n";

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

std::string parseRouterBlock(const std::string& block_content) {
    std::string js_code;
    std::map<std::string, std::string> routes;
    std::string mode = "hash";
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
                value_end++;
            }
        } else if (block_content.substr(value_start, 2) == "{{") {
            value_end = block_content.find("}}", value_start);
             if (value_end != std::string::npos) {
                value_end += 2;
            }
        } else {
            value_end = block_content.find_first_of(", \t\n\r", value_start);
        }
        if (value_end == std::string::npos) {
            value_end = block_content.length();
        }
        std::string value = block_content.substr(value_start, value_end - value_start);
        value.erase(value.find_last_not_of(" \t\n\r,") + 1);
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

    if (routes.empty()) return "console.error(\"CHTL JS Router Error: No routes were defined.\");\n";
    if (mode != "hash") return "console.error(\"CHTL JS Router Error: Only 'hash' mode is currently supported.\");\n";

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

std::string parseScriptLoaderBlock(const std::string& block_content) {
    std::string js_code = "(function() {\n";
    size_t pos = 0;
    int script_counter = 0;
    while ((pos = block_content.find("load:", pos)) != std::string::npos) {
        size_t value_start = block_content.find_first_not_of(" \t\n\r", pos + 5);
        if (value_start == std::string::npos) break;
        size_t value_end = block_content.find_first_of(";\n", value_start);
        if (value_end == std::string::npos) {
            value_end = block_content.length();
        }
        std::string path_list_str = block_content.substr(value_start, value_end - value_start);
        size_t path_pos = 0;
        while (path_pos < path_list_str.length()) {
            size_t path_start = path_list_str.find_first_not_of(" \t\n\r,", path_pos);
            if (path_start == std::string::npos) break;
            size_t path_end = path_list_str.find_first_of(" \t\n\r,", path_start);
            if (path_end == std::string::npos) {
                path_end = path_list_str.length();
            }
            std::string script_path = path_list_str.substr(path_start, path_end - path_start);
            if ((script_path.front() == '\'' && script_path.back() == '\'') || (script_path.front() == '"' && script_path.back() == '"')) {
                script_path = script_path.substr(1, script_path.length() - 2);
            }
            js_code += "  const script" + std::to_string(script_counter) + " = document.createElement('script');\n";
            js_code += "  script" + std::to_string(script_counter) + ".src = '" + script_path + "';\n";
            js_code += "  document.head.appendChild(script" + std::to_string(script_counter) + ");\n";
            script_counter++;
            path_pos = path_end;
        }
        pos = value_end;
    }
    js_code += "})();\n";
    return js_code;
}