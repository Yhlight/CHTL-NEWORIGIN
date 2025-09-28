#include "Tokenizer.h"
#include <unordered_set>
#include <cctype>

std::vector<Token> Tokenizer::tokenize(const std::string& source) {
    std::vector<Token> tokens;
    size_t pos = 0;

    const std::unordered_set<std::string> chtl_keywords = {
        "Animate", "Listen", "Delegate", "Router", "ScriptLoader", "iNeverAway"
    };

    while (pos < source.length()) {
        char current_char = source[pos];

        if (isspace(current_char)) {
            size_t start = pos;
            while (pos < source.length() && isspace(source[pos])) pos++;
            tokens.push_back({TokenType::WHITESPACE, source.substr(start, pos - start)});
            continue;
        }

        if (current_char == '/' && pos + 1 < source.length()) {
            if (source[pos + 1] == '/') { // Single-line comment
                size_t start = pos;
                pos += 2;
                while (pos < source.length() && source[pos] != '\n') pos++;
                tokens.push_back({TokenType::COMMENT, source.substr(start, pos - start)});
                continue;
            }
            if (source[pos + 1] == '*') { // Multi-line comment
                size_t start = pos;
                pos += 2;
                while (pos + 1 < source.length() && !(source[pos] == '*' && source[pos+1] == '/')) pos++;
                pos += 2;
                tokens.push_back({TokenType::COMMENT, source.substr(start, pos - start)});
                continue;
            }
        }

        if (current_char == '\'' || current_char == '"') {
            size_t start = pos;
            char quote_type = current_char;
            pos++;
            while (pos < source.length() && source[pos] != quote_type) {
                if (source[pos] == '\\') pos++;
                pos++;
            }
            pos++;
            tokens.push_back({TokenType::STRING_LITERAL, source.substr(start, pos - start)});
            continue;
        }

        if (isdigit(current_char)) {
            size_t start = pos;
            while (pos < source.length() && isdigit(source[pos])) pos++;
            tokens.push_back({TokenType::NUMBER_LITERAL, source.substr(start, pos - start)});
            continue;
        }

        if (current_char == '{' && pos + 1 < source.length() && source[pos+1] == '{') {
            size_t start = pos;
            pos += 2;
            while (pos + 1 < source.length() && !(source[pos] == '}' && source[pos+1] == '}')) pos++;
            pos += 2;
            tokens.push_back({TokenType::CHTL_SELECTOR, source.substr(start, pos - start)});
            continue;
        }

        if (current_char == '-' && pos + 1 < source.length() && source[pos+1] == '>') {
            tokens.push_back({TokenType::CHTL_OPERATOR, "->"});
            pos += 2;
            continue;
        }

        if (isalpha(current_char)) {
            size_t start = pos;
            while (pos < source.length() && isalnum(source[pos])) pos++;
            std::string word = source.substr(start, pos - start);
            if (word == "Vir") tokens.push_back({TokenType::VIR_KEYWORD, word});
            else if (chtl_keywords.count(word)) tokens.push_back({TokenType::CHTL_KEYWORD, word});
            else tokens.push_back({TokenType::IDENTIFIER, word});
            continue;
        }

        if (std::string("=;:,{}[]()<>").find(current_char) != std::string::npos) {
            tokens.push_back({TokenType::SYMBOL, std::string(1, current_char)});
            pos++;
            continue;
        }

        tokens.push_back({TokenType::UNKNOWN, std::string(1, current_char)});
        pos++;
    }

    return tokens;
}