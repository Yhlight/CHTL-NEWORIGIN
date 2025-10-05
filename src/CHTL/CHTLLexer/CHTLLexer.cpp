#include "CHTLLexer.h"

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '/' && i + 1 < input.length()) {
            if (input[i + 1] == '/') {
                // Single-line comment
                size_t start = i;
                while (i < input.length() && input[i] != '\n') {
                    i++;
                }
                tokens.push_back({TokenType::SINGLE_LINE_COMMENT, input.substr(start, i - start)});
            } else if (input[i + 1] == '*') {
                // Multi-line comment
                size_t start = i;
                i += 2;
                while (i + 1 < input.length() && (input[i] != '*' || input[i + 1] != '/')) {
                    i++;
                }
                i++; // move past '*'
                i++; // move past '/'
                tokens.push_back({TokenType::MULTI_LINE_COMMENT, input.substr(start, i - start)});
            }
        } else if (input[i] == '#') {
            if (i + 1 < input.length() && input[i+1] == ' ') {
                // Generator comment
                size_t start = i;
                while (i < input.length() && input[i] != '\n') {
                    i++;
                }
                tokens.push_back({TokenType::GENERATOR_COMMENT, input.substr(start, i - start)});
            } else {
                 // Not a valid generator comment, treat as unknown for now
                size_t start = i;
                while (i < input.length() && input[i] != '\n' && input[i] != ' ' && input[i] != '\t') {
                    i++;
                }
                tokens.push_back({TokenType::UNKNOWN, input.substr(start, i - start)});
            }
        }
        // Ignoring other characters for now
    }
    return tokens;
}