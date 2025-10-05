#include "CHTLLexer.h"

#include <cctype>

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < input.length(); ++i) {
        if (isspace(input[i])) {
            continue; // Skip whitespace
        }

        if (input[i] == '/' && i + 1 < input.length()) {
            if (input[i + 1] == '/') {
                size_t start = i;
                while (i < input.length() && input[i] != '\n') {
                    i++;
                }
                tokens.push_back({TokenType::SINGLE_LINE_COMMENT, input.substr(start, i - start)});
                continue;
            } else if (input[i + 1] == '*') {
                size_t start = i;
                i += 2;
                while (i + 1 < input.length() && (input[i] != '*' || input[i + 1] != '/')) {
                    i++;
                }
                i++;
                i++;
                tokens.push_back({TokenType::MULTI_LINE_COMMENT, input.substr(start, i - start)});
                continue;
            }
        }

        if (input[i] == '#') {
            if (i + 1 < input.length() && input[i + 1] == ' ') {
                size_t start = i;
                while (i < input.length() && input[i] != '\n') {
                    i++;
                }
                tokens.push_back({TokenType::GENERATOR_COMMENT, input.substr(start, i - start)});
            } else {
                size_t start = i;
                while (i < input.length() && !isspace(input[i])) {
                    i++;
                }
                tokens.push_back({TokenType::UNQUOTED_LITERAL, input.substr(start, i - start)});
            }
            continue;
        }

        if (isalpha(input[i])) {
            size_t start = i;
            while (i < input.length() && isalnum(input[i])) {
                i++;
            }
            std::string value = input.substr(start, i - start);
            if (value == "text") {
                tokens.push_back({TokenType::TEXT_KEYWORD, value});
            } else {
                tokens.push_back({TokenType::UNQUOTED_LITERAL, value});
            }
            i--; // Decrement to re-evaluate the character that broke the loop
            continue;
        }

        if (input[i] == '"' || input[i] == '\'') {
            char quote_type = input[i];
            size_t start = i;
            i++;
            while (i < input.length() && input[i] != quote_type) {
                i++;
            }
            i++; // Include the closing quote
            tokens.push_back({TokenType::STRING_LITERAL, input.substr(start, i - start)});
            i--; // Decrement to re-evaluate the character that broke the loop
            continue;
        }

        switch (input[i]) {
            case '{': tokens.push_back({TokenType::LEFT_BRACE, "{"}); break;
            case '}': tokens.push_back({TokenType::RIGHT_BRACE, "}"}); break;
            case ':': tokens.push_back({TokenType::COLON, ":"}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";"}); break;
            default:
                // Handle unquoted literals that don't start with a letter
                if (!isspace(input[i])) {
                    size_t start = i;
                    while (i < input.length() && !isspace(input[i]) && input[i] != '{' && input[i] != '}' && input[i] != ':' && input[i] != ';') {
                        i++;
                    }
                    tokens.push_back({TokenType::UNQUOTED_LITERAL, input.substr(start, i - start)});
                    i--; // Decrement to re-evaluate the character that broke the loop
                }
                break;
        }
    }
    return tokens;
}