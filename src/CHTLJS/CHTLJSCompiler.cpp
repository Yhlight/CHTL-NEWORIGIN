#include "CHTLJSCompiler.h"
#include "../Util/TokenUtils.h"
#include <iostream>
#include <map>
#include <vector>
#include <cctype>

// Forward declarations for all token-based parsers
std::string compileRouterBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out);
std::string compileAnimateBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out);
std::string compileDelegateBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out);
std::string compileListenBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out);
std::string compileScriptLoaderBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out);
std::string compileSelector(const Token& token);
std::string parseKeyframeObject(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out);

// Main compile method with robust lookahead logic
std::string CHTLJSCompiler::compile(std::vector<Token>& tokens) {
    std::vector<Token> compiled_tokens;
    for (size_t i = 0; i < tokens.size(); ++i) {
        size_t end_index = i;
        std::string compiled_js;

        if (tokens[i].type == TokenType::CHTL_KEYWORD) {
            if (tokens[i].text == "Router") compiled_js = compileRouterBlock(tokens, i, end_index);
            else if (tokens[i].text == "Animate") compiled_js = compileAnimateBlock(tokens, i, end_index);
            else if (tokens[i].text == "ScriptLoader") compiled_js = compileScriptLoaderBlock(tokens, i, end_index);
        }
        else if (tokens[i].type == TokenType::CHTL_SELECTOR || tokens[i].type == TokenType::IDENTIFIER) {
            size_t next_idx = nextMeaningfulToken(tokens, i);
            if (next_idx != std::string::npos && tokens[next_idx].type == TokenType::CHTL_OPERATOR) {
                size_t keyword_idx = nextMeaningfulToken(tokens, next_idx);
                if (keyword_idx != std::string::npos && tokens[keyword_idx].type == TokenType::CHTL_KEYWORD) {
                    if (tokens[keyword_idx].text == "Delegate") compiled_js = compileDelegateBlock(tokens, i, end_index);
                    else if (tokens[keyword_idx].text == "Listen") compiled_js = compileListenBlock(tokens, i, end_index);
                }
            } else if (tokens[i].type == TokenType::CHTL_SELECTOR) {
                 compiled_js = compileSelector(tokens[i]);
            }
        }

        if (!compiled_js.empty()) {
            compiled_tokens.push_back({TokenType::JS_BLOCK, compiled_js});
            i = end_index;
        } else {
            compiled_tokens.push_back(tokens[i]);
        }
    }

    std::string final_code;
    for (const auto& token : compiled_tokens) {
        final_code += token.text;
    }
    return final_code;
}

// --- HELPER IMPLEMENTATIONS ---

std::string getTokenValue(const Token& token) {
    if (token.type == TokenType::STRING_LITERAL) {
        if (token.text.length() >= 2) return token.text.substr(1, token.text.length() - 2);
    }
    return token.text;
}

std::string compileSelector(const Token& token) {
    if (token.type != TokenType::CHTL_SELECTOR) return token.text;
    std::string inner = token.text.substr(2, token.text.length() - 4);
    if (inner[0] == '.' || inner[0] == '#') return "document.querySelector('" + inner + "')";
    return "document.querySelector('" + inner + "')";
}

std::string parseKeyframeObject(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out) {
    if (start_index >= tokens.size() || tokens[start_index].text != "{") return "{}";
    size_t close_brace_idx = findMatchingBraceToken(tokens, start_index);
    if(close_brace_idx == std::string::npos) return "{}";

    std::string js_object = "{";
    for(size_t i = start_index + 1; i < close_brace_idx; ++i) {
        i = nextMeaningfulToken(tokens, i-1);
        if(i >= close_brace_idx) break;
        if(tokens[i].type == TokenType::IDENTIFIER) {
            std::string key = tokens[i].text;
            size_t colon_idx = nextMeaningfulToken(tokens, i);
            size_t value_idx = nextMeaningfulToken(tokens, colon_idx);
            if(value_idx < close_brace_idx && tokens[colon_idx].text == ":") {
                if(key == "at") js_object += "offset:" + getTokenValue(tokens[value_idx]) + ",";
                else {
                    size_t hyphen_pos = key.find('-');
                    while (hyphen_pos != std::string::npos) {
                        key.erase(hyphen_pos, 1);
                        if (hyphen_pos < key.length()) key[hyphen_pos] = toupper(key[hyphen_pos]);
                        hyphen_pos = key.find('-');
                    }
                    js_object += key + ":" + tokens[value_idx].text + ",";
                }
                i = value_idx;
            }
        }
    }
    js_object += "}";
    end_index_out = close_brace_idx;
    return js_object;
}

std::string compileAnimateBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out) {
    size_t open_brace_idx = nextMeaningfulToken(tokens, start_index);
    if (open_brace_idx == std::string::npos || tokens[open_brace_idx].text != "{") return "";
    size_t close_brace_idx = findMatchingBraceToken(tokens, open_brace_idx);
    if (close_brace_idx == std::string::npos) return "";

    std::string target, duration, callback;
    std::string options_js = "{";
    std::vector<std::string> keyframes_js;

    for (size_t i = open_brace_idx + 1; i < close_brace_idx; ++i) {
        i = nextMeaningfulToken(tokens, i - 1);
        if (i >= close_brace_idx) break;

        if (tokens[i].type == TokenType::IDENTIFIER) {
            std::string key = tokens[i].text;
            size_t colon_idx = nextMeaningfulToken(tokens, i);
            size_t value_idx = nextMeaningfulToken(tokens, colon_idx);

            if (value_idx < close_brace_idx && tokens[colon_idx].text == ":") {
                const auto& value_token = tokens[value_idx];
                std::string value = getTokenValue(value_token);
                if (key == "target") target = value_token.type == TokenType::CHTL_SELECTOR ? compileSelector(value_token) : value;
                else if (key == "duration") { duration = value; options_js += "duration:" + value + ","; }
                else if (key == "callback") callback = value;
                else if (key == "easing" || key == "direction") options_js += key + ":'" + value + "',";
                else if (key == "loop") {
                    if (value == "-1") options_js += "iterations:Infinity,";
                    else options_js += "iterations:" + value + ",";
                }
                else if (key == "begin") { size_t frame_end; keyframes_js.insert(keyframes_js.begin(), parseKeyframeObject(tokens, value_idx, frame_end)); i = frame_end; }
                else if (key == "end") { size_t frame_end; keyframes_js.push_back(parseKeyframeObject(tokens, value_idx, frame_end)); i = frame_end; }
                else if (key == "when") {
                    size_t array_end = std::string::npos;
                    int balance = 0;
                    for(size_t k=value_idx; k<close_brace_idx; ++k){
                        if(tokens[k].text == "[") balance++; if(tokens[k].text == "]") balance--;
                        if(balance == 0) { array_end = k; break; }
                    }
                    if(array_end != std::string::npos) {
                        for(size_t k=value_idx+1; k<array_end; ++k){
                             k = nextMeaningfulToken(tokens, k-1); if(k >= array_end) break;
                             if(tokens[k].text == "{") { size_t frame_end; keyframes_js.push_back(parseKeyframeObject(tokens, k, frame_end)); k = frame_end; }
                        }
                        i = array_end;
                    }
                }
            }
        }
    }
    options_js += "}";
    if(target.empty() || duration.empty()) return "console.error('CHTL Animate: target and duration required');";

    std::string js_code = "const target_elem=" + target + ";if(target_elem){const anim=target_elem.animate([";
    for(size_t i=0; i<keyframes_js.size(); ++i) js_code += keyframes_js[i] + (i == keyframes_js.size() - 1 ? "" : ",");
    js_code += "], " + options_js + ");";
    if(!callback.empty()) js_code += "anim.finished.then(" + callback + ");";
    js_code += "}";

    end_index_out = close_brace_idx;
    size_t next_token = nextMeaningfulToken(tokens, close_brace_idx);
    if(next_token != std::string::npos && tokens[next_token].text == ";") end_index_out = next_token;
    return js_code;
}

std::string compileDelegateBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out) {
    std::string parent_selector = tokens[start_index].type == TokenType::CHTL_SELECTOR ? compileSelector(tokens[start_index]) : tokens[start_index].text;
    size_t keyword_idx = nextMeaningfulToken(tokens, start_index + 1);
    size_t open_brace_idx = nextMeaningfulToken(tokens, keyword_idx);
    if (open_brace_idx == std::string::npos || tokens[open_brace_idx].text != "{") return "";
    size_t close_brace_idx = findMatchingBraceToken(tokens, open_brace_idx);
    if (close_brace_idx == std::string::npos) return "";

    std::string target_selector;
    std::map<std::string, std::string> event_handlers;

    for (size_t i = open_brace_idx + 1; i < close_brace_idx; ++i) {
        i = nextMeaningfulToken(tokens, i - 1);
        if (i >= close_brace_idx) break;
        if (tokens[i].type == TokenType::IDENTIFIER) {
            std::string key = tokens[i].text;
            size_t colon_idx = nextMeaningfulToken(tokens, i);
            size_t value_idx = nextMeaningfulToken(tokens, colon_idx);
            if (value_idx < close_brace_idx && tokens[colon_idx].text == ":") {
                if (key == "target") target_selector = getTokenValue(tokens[value_idx]);
                else {
                    size_t handler_end = value_idx;
                    int balance = 0;
                    for(size_t k=value_idx; k < close_brace_idx; ++k) {
                        if(tokens[k].text == "{") balance++;
                        if(tokens[k].text == "}") balance--;
                        if(balance == 0 && (tokens[k].text == "," || k == close_brace_idx -1)) { handler_end = k; break; }
                        handler_end = k;
                    }
                    std::string handler_code;
                    bool is_inline_func = false;
                    for(size_t k=value_idx; k <= handler_end; ++k) {
                        if (tokens[k].text == "function") is_inline_func = true;
                        handler_code += tokens[k].text;
                    }

                    if (is_inline_func) {
                        event_handlers[key] = "(" + handler_code + ")(e)";
                    } else {
                        event_handlers[key] = handler_code + "(e)";
                    }
                    i = handler_end;
                }
            }
        }
    }

    if (target_selector.empty()) return "console.error('CHTL Delegate Error: target is required');";

    std::string js_code = "const parent_elem=" + parent_selector + ";if(parent_elem){";
    for(const auto& pair : event_handlers) {
        js_code += "parent_elem.addEventListener('" + pair.first + "',(e)=>{if(e.target.matches(\"" + target_selector + "\")){" + pair.second + ";}});";
    }
    js_code += "}";

    end_index_out = close_brace_idx;
    size_t next_token = nextMeaningfulToken(tokens, close_brace_idx);
    if(next_token != std::string::npos && tokens[next_token].text == ";") end_index_out = next_token;
    return js_code;
}

std::string compileListenBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out) {
    return compileDelegateBlock(tokens, start_index, end_index_out);
}

std::string compileRouterBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out) {
    size_t open_brace_idx = nextMeaningfulToken(tokens, start_index);
    if (open_brace_idx == std::string::npos || tokens[open_brace_idx].text != "{") return "";
    size_t close_brace_idx = findMatchingBraceToken(tokens, open_brace_idx);
    if (close_brace_idx == std::string::npos) return "";

    std::map<std::string, Token> routes;
    std::string mode = "hash";
    std::string current_url;

    for (size_t i = open_brace_idx + 1; i < close_brace_idx; ++i) {
        i = nextMeaningfulToken(tokens, i - 1);
        if (i >= close_brace_idx) break;
        if (tokens[i].type == TokenType::IDENTIFIER) {
            std::string key = tokens[i].text;
            size_t colon_idx = nextMeaningfulToken(tokens, i);
            size_t value_idx = nextMeaningfulToken(tokens, colon_idx);
            if (value_idx < close_brace_idx && tokens[colon_idx].text == ":") {
                const Token& value_token = tokens[value_idx];
                if (key == "mode") mode = getTokenValue(value_token);
                else if (key == "url") current_url = getTokenValue(value_token);
                else if (key == "page" && value_token.type == TokenType::CHTL_SELECTOR && !current_url.empty()) {
                    routes[current_url] = value_token;
                    current_url.clear();
                }
                i = value_idx;
            }
        }
    }

    std::string js_code;
    if (routes.empty()) js_code = "console.error(\"CHTL JS Router Error: No routes were defined.\");";
    else {
        js_code += "const routes = {";
        for(const auto& route : routes) js_code += "  '" + route.first + "': " + compileSelector(route.second) + ",";
        js_code += "};";
        if (mode == "history") js_code += R"JS(const navigate = (path) => { window.history.pushState({ path }, '', path); const page = routes[path]; Object.values(routes).forEach(p => { if(p) p.style.display = 'none'; }); if (page) page.style.display = 'block'; }; window.addEventListener('popstate', (e) => { const path = e.state ? e.state.path : '/'; const page = routes[path]; Object.values(routes).forEach(p => { if(p) p.style.display = 'none'; }); if (page) page.style.display = 'block'; }); document.addEventListener('click', (e) => { const anchor = e.target.closest('a'); if (anchor) { const href = anchor.getAttribute('href'); if (href && href.startsWith('/')) { e.preventDefault(); navigate(href); } } }); navigate(window.location.pathname);)JS";
        else js_code += R"JS(function navigate() { const path = window.location.hash.slice(1) || '/'; const page = routes[path]; for (const key in routes) { if (routes[key]) routes[key].style.display = 'none'; } if (page) { page.style.display = 'block'; } } window.addEventListener('hashchange', navigate); navigate();)JS";
    }

    end_index_out = close_brace_idx;
    size_t next_token = nextMeaningfulToken(tokens, close_brace_idx);
    if(next_token != std::string::npos && tokens[next_token].text == ";") end_index_out = next_token;
    return js_code;
}

std::string compileScriptLoaderBlock(const std::vector<Token>& tokens, size_t start_index, size_t& end_index_out) {
    size_t open_brace_idx = nextMeaningfulToken(tokens, start_index);
    if (open_brace_idx == std::string::npos || tokens[open_brace_idx].text != "{") return "";
    size_t close_brace_idx = findMatchingBraceToken(tokens, open_brace_idx);
    if (close_brace_idx == std::string::npos) return "";

    std::string js_code = "(function() {";
    int script_counter = 0;
    for (size_t i = open_brace_idx + 1; i < close_brace_idx; ++i) {
        if (tokens[i].type == TokenType::IDENTIFIER && tokens[i].text == "load") {
            size_t colon_idx = nextMeaningfulToken(tokens, i);
            if(colon_idx >= close_brace_idx || tokens[colon_idx].text != ":") continue;
            size_t value_idx = nextMeaningfulToken(tokens, colon_idx);
            while(value_idx < close_brace_idx) {
                std::string path = getTokenValue(tokens[value_idx]);
                js_code += "const script" + std::to_string(script_counter) + "=document.createElement('script');";
                js_code += "script" + std::to_string(script_counter) + ".src='" + path + "';";
                js_code += "document.head.appendChild(script" + std::to_string(script_counter++) + ");";
                size_t next_val_idx = nextMeaningfulToken(tokens, value_idx);
                if(next_val_idx >= close_brace_idx || tokens[next_val_idx].text != ",") break;
                value_idx = nextMeaningfulToken(tokens, next_val_idx);
            }
        }
    }
    js_code += "})();";
    end_index_out = close_brace_idx;
    size_t next_token = nextMeaningfulToken(tokens, close_brace_idx);
    if(next_token != std::string::npos && tokens[next_token].text == ";") end_index_out = next_token;
    return js_code;
}