#include "CHTLJSParser.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

// Basic string trim utility
void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parse(const std::string& source) {
    std::string content = source;
    trim(content);

    // Use an if-else if structure to ensure only one block type is parsed.
    // Order is important: more specific/complex patterns should come first.

    // Check for Vir declaration
    if (content.rfind("Vir ", 0) == 0) {
        size_t name_start = 4;
        while(name_start < content.length() && isspace(content[name_start])) {
            name_start++;
        }
        size_t eq_pos = content.find('=', name_start);
        if (eq_pos != std::string::npos) {
            std::string name = content.substr(name_start, eq_pos - name_start);
            trim(name);

            std::string assigned_code = content.substr(eq_pos + 1);
            trim(assigned_code);
             if (!assigned_code.empty() && assigned_code.back() == ';') {
                assigned_code.pop_back();
            }
            auto assigned_node = parse(assigned_code);
            if (assigned_node) {
                return std::make_unique<VirtualObjectNode>(name, std::move(assigned_node));
            }
        }
    }
    // Check for Router block syntax
    else if (content.rfind("Router {", 0) == 0) {
        size_t block_start = content.find('{');
        size_t block_end = content.rfind('}');
        if (block_start != std::string::npos && block_end != std::string::npos) {
            std::string block_content = content.substr(block_start + 1, block_end - block_start - 1);
            trim(block_content);

            std::map<std::string, std::string> routes;
            std::string url, page;

            std::regex url_regex(R"---(url\s*:\s*"(.*?)"\s*,?)---");
            std::smatch match;
            if (std::regex_search(block_content, match, url_regex)) {
                url = match[1].str();
            }

            std::regex page_regex(R"(page\s*:\s*(\{\{.*?\}\})\s*,?)");
            if (std::regex_search(block_content, match, page_regex)) {
                page = match[1].str();
            }

            if (!url.empty() && !page.empty()) {
                routes[url] = page;
            }

            return std::make_unique<RouterNode>(routes);
        }
    }
    // Check for Animate block syntax
    else if (content.find("Animate {") != std::string::npos) {
        size_t animate_pos = content.find("Animate {");
        size_t block_start = content.find('{', animate_pos);
        size_t block_end = content.rfind('}');
        if (block_start != std::string::npos && block_end != std::string::npos) {
            std::string block_content = content.substr(block_start + 1, block_end - block_start - 1);
            trim(block_content);

            std::map<std::string, std::string> properties;
            size_t current_pos = 0;
            while (current_pos < block_content.length()) {
                size_t colon_pos = block_content.find(':', current_pos);
                if (colon_pos == std::string::npos) break;

                std::string key = block_content.substr(current_pos, colon_pos - current_pos);
                trim(key);

                size_t value_start = colon_pos + 1;
                while (value_start < block_content.length() && isspace(block_content[value_start])) {
                    value_start++;
                }

                size_t value_end = value_start;
                int brace_level = 0;
                int bracket_level = 0;
                if (value_start < block_content.length() && (block_content[value_start] == '{' || block_content[value_start] == '[')) {
                    if(block_content[value_start] == '{') brace_level++; else bracket_level++;

                    value_end = value_start + 1;
                    while (value_end < block_content.length() && (brace_level > 0 || bracket_level > 0)) {
                        char c = block_content[value_end];
                        if (c == '{') brace_level++;
                        else if (c == '}') brace_level--;
                        else if (c == '[') bracket_level++;
                        else if (c == ']') bracket_level--;
                        value_end++;
                    }
                } else {
                    value_end = block_content.find(',', value_start);
                    if (value_end == std::string::npos) {
                        value_end = block_content.length();
                    }
                }

                std::string value = block_content.substr(value_start, value_end - value_start);
                trim(value);
                 if (!value.empty() && value.back() == ',') {
                        value.pop_back();
                        trim(value);
                    }
                properties[key] = value;
                current_pos = value_end + 1;
            }
            return std::make_unique<AnimateNode>(properties);
        }
    }
    // Check for ScriptLoader syntax
    else if (content.rfind("ScriptLoader {", 0) == 0) {
        std::vector<std::string> files;
        size_t start = content.find('{');
        size_t end = content.rfind('}');
        if (start != std::string::npos && end != std::string::npos) {
            std::string block_content = content.substr(start + 1, end - start - 1);
            std::stringstream ss(block_content);
            std::string line;
            while(std::getline(ss, line)) {
                trim(line);
                if (line.rfind("load:", 0) == 0) {
                    std::string file_path = line.substr(5);
                    trim(file_path);
                    file_path.erase(std::remove(file_path.begin(), file_path.end(), '\"'), file_path.end());
                    file_path.erase(std::remove(file_path.begin(), file_path.end(), ','), file_path.end());
                    trim(file_path);
                    if (!file_path.empty()) {
                        files.push_back(file_path);
                    }
                }
            }
        }
        return std::make_unique<ScriptLoaderNode>(files);
    }
    // Check for Delegate block syntax
    else if (content.find("->Delegate") != std::string::npos) {
        size_t delegate_pos = content.find("->Delegate");
        size_t parent_selector_start = content.find("{{");
        size_t parent_selector_end = content.find("}}");
        if (parent_selector_start == 0 && parent_selector_end != std::string::npos && delegate_pos > parent_selector_end) {
            std::string parent_selector = content.substr(parent_selector_start + 2, parent_selector_end - parent_selector_start - 2);
            size_t block_start = content.find('{', delegate_pos);
            size_t block_end = content.rfind('}');
            if (block_start != std::string::npos && block_end != std::string::npos) {
                std::string block_content = content.substr(block_start + 1, block_end - block_start - 1);
                trim(block_content);

                std::regex target_regex(R"(target\s*:\s*\{\{(.*?)\}\}\s*,?)");
                std::smatch match;
                std::string target_selector;
                if (std::regex_search(block_content, match, target_regex)) {
                    target_selector = match[1].str();
                    block_content = std::regex_replace(block_content, target_regex, "");
                    trim(block_content);
                }

                std::map<std::string, std::string> events;
                size_t current_pos = 0;
                 while (current_pos < block_content.length()) {
                    size_t colon_pos = block_content.find(':', current_pos);
                    if (colon_pos == std::string::npos) break;
                    std::string event_name = block_content.substr(current_pos, colon_pos - current_pos);
                    trim(event_name);
                    size_t callback_start = colon_pos + 1;
                    while(callback_start < block_content.length() && isspace(block_content[callback_start])) {
                        callback_start++;
                    }
                    int brace_level = 0;
                    size_t callback_end = callback_start;
                    while(callback_end < block_content.length()) {
                        char c = block_content[callback_end];
                        if (c == '{') brace_level++;
                        else if (c == '}') brace_level--;
                        else if (c == ',' && brace_level == 0) break;
                        callback_end++;
                    }
                    std::string callback = block_content.substr(callback_start, callback_end - callback_start);
                    trim(callback);
                    if (!callback.empty() && callback.back() == ',') {
                        callback.pop_back();
                        trim(callback);
                    }
                    if (!event_name.empty()) {
                        events[event_name] = callback;
                    }
                    current_pos = callback_end + 1;
                }
                return std::make_unique<DelegateNode>(parent_selector, target_selector, events);
            }
        }
    }
    // Check for Listen block syntax (chained or standalone)
    else if (content.find("Listen {") != std::string::npos) {
        bool is_chained_listen = (content.find("->Listen") != std::string::npos);
        std::string selector;
        size_t block_start;

        if (is_chained_listen) {
            size_t selector_start = content.find("{{");
            size_t selector_end = content.find("}}");
            size_t arrow_pos = content.find("->Listen");
             if (selector_start == 0 && selector_end != std::string::npos && arrow_pos > selector_end) {
                selector = content.substr(selector_start + 2, selector_end - selector_start - 2);
                block_start = content.find('{', arrow_pos);
            } else { return nullptr; }
        } else { // Standalone
            selector = "";
            block_start = content.find('{');
        }

        size_t block_end = content.rfind('}');
        if (block_start != std::string::npos && block_end != std::string::npos) {
            std::string block_content = content.substr(block_start + 1, block_end - block_start - 1);
            trim(block_content);
            std::map<std::string, std::string> events;
            size_t current_pos = 0;
            while (current_pos < block_content.length()) {
                size_t colon_pos = block_content.find(':', current_pos);
                if (colon_pos == std::string::npos) break;
                std::string event_name = block_content.substr(current_pos, colon_pos - current_pos);
                trim(event_name);
                size_t callback_start = colon_pos + 1;
                while(callback_start < block_content.length() && isspace(block_content[callback_start])) {
                    callback_start++;
                }
                int brace_level = 0;
                size_t callback_end = callback_start;
                while(callback_end < block_content.length()) {
                    char c = block_content[callback_end];
                    if (c == '{') brace_level++;
                    else if (c == '}') brace_level--;
                    else if (c == ',' && brace_level == 0) break;
                    callback_end++;
                }
                std::string callback = block_content.substr(callback_start, callback_end - callback_start);
                trim(callback);
                if (!callback.empty() && callback.back() == ',') {
                    callback.pop_back();
                    trim(callback);
                }
                events[event_name] = callback;
                current_pos = callback_end + 1;
            }
            return std::make_unique<ListenNode>(selector, events);
        }
    }
    // Check for Event Binding (&->) syntax
    else if (content.find(" &-> ") != std::string::npos) {
        size_t event_bind_pos = content.find(" &-> ");
        size_t selector_start = content.find("{{");
        size_t selector_end = content.find("}}");
        if (selector_start == 0 && selector_end != std::string::npos && event_bind_pos > selector_end) {
            std::string selector = content.substr(selector_start + 2, selector_end - selector_start - 2);
            size_t event_start = event_bind_pos + 5;
            size_t colon_pos = content.find(':', event_start);
            if (colon_pos != std::string::npos) {
                std::string event_name = content.substr(event_start, colon_pos - event_start);
                trim(event_name);
                std::string callback = content.substr(colon_pos + 1);
                trim(callback);
                if (!callback.empty() && callback.back() == ';') {
                    callback.pop_back();
                    trim(callback);
                }
                return std::make_unique<EventBindingNode>(selector, event_name, callback);
            }
        }
    }
    // Check for Enhanced Selector syntax
    else if (content.find("{{") == 0) {
        size_t selector_end = content.find("}}");
        size_t arrow = content.find("->");
        if (selector_end != std::string::npos && arrow == selector_end + 2) {
            std::string selector = content.substr(2, selector_end - 2);
            std::string expression = content.substr(arrow + 2);
            if (!expression.empty() && expression.back() == ';') {
                expression.pop_back();
            }
            return std::make_unique<EnhancedSelectorNode>(selector, expression);
        }
    }

    return nullptr;
}