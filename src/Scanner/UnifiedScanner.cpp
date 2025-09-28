#include "UnifiedScanner.h"
#include "Tokenizer.h"
#include <algorithm>
#include <cctype>

std::string UnifiedScanner::scan(const std::string& source) {
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(source);

    std::string result;
    std::string js_buffer;

    auto flush_js_buffer = [&]() {
        if (!js_buffer.empty()) {
            result += addPlaceholder(js_buffer);
            js_buffer.clear();
        }
    };

    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::CHTL_KEYWORD:
            case TokenType::CHTL_SELECTOR:
            case TokenType::CHTL_OPERATOR:
            case TokenType::VIR_KEYWORD:
                flush_js_buffer();
                result += token.text;
                break;

            case TokenType::JS_BLOCK:
            case TokenType::WHITESPACE:
            case TokenType::COMMENT:
            case TokenType::STRING_LITERAL:
            case TokenType::UNKNOWN:
            default:
                js_buffer += token.text;
                break;
        }
    }

    flush_js_buffer(); // Add any remaining JS at the end

    return result;
}

const std::map<std::string, std::string>& UnifiedScanner::getPlaceholders() const {
    return placeholder_map;
}

std::string UnifiedScanner::addPlaceholder(const std::string& code) {
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