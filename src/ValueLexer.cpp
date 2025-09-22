#include "ValueLexer.h"
#include <cctype>

ValueLexer::ValueLexer(const std::string& source) : source(source) {}

std::vector<ValueToken> ValueLexer::tokenize() {
    std::vector<ValueToken> tokens;
    while (!isAtEnd()) {
        tokens.push_back(nextToken());
    }
    tokens.push_back({ValueTokenType::EndOfValue, ""});
    return tokens;
}

ValueToken ValueLexer::nextToken() {
    skipWhitespace();
    if (isAtEnd()) {
        return {ValueTokenType::EndOfValue, ""};
    }

    char c = peek();

    if (isdigit(c) || c == '.') {
        std::string value;
        while (!isAtEnd() && (isdigit(peek()) || peek() == '.')) {
            value += advance();
        }
        return {ValueTokenType::Number, value};
    }

    if (isalpha(c)) {
        std::string value;
        while (!isAtEnd() && isalpha(peek())) {
            value += advance();
        }
        return {ValueTokenType::Unit, value};
    }

    if (c == '+' || c == '-' || c == '*' || c == '/') {
        return {ValueTokenType::Operator, std::string(1, advance())};
    }

    advance();
    return {ValueTokenType::Unknown, std::string(1, c)};
}

void ValueLexer::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

char ValueLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char ValueLexer::advance() {
    current++;
    return source[current - 1];
}

bool ValueLexer::isAtEnd() {
    return current >= source.length();
}
