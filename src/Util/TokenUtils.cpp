#include "TokenUtils.h"

size_t findMatchingBraceToken(const std::vector<Token>& tokens, size_t start_index) {
    if (start_index >= tokens.size() || tokens[start_index].text != "{") {
        return std::string::npos;
    }
    int balance = 1;
    for (size_t i = start_index + 1; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::SYMBOL) {
            if (tokens[i].text == "{") balance++;
            if (tokens[i].text == "}") balance--;
        }
        if (balance == 0) return i;
    }
    return std::string::npos;
}

size_t nextMeaningfulToken(const std::vector<Token>& tokens, size_t current_index) {
    size_t i = current_index + 1;
    while (i < tokens.size()) {
        if (tokens[i].type != TokenType::WHITESPACE && tokens[i].type != TokenType::COMMENT) {
            return i;
        }
        i++;
    }
    return std::string::npos;
}