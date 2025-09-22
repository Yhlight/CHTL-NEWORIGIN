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
        while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
            value += advance();
        }
        // A unit is just a specific type of identifier, the expression parser can distinguish
        return {ValueTokenType::Identifier, value};
    }

    if (c == '+' || c == '-' || c == '*' || c == '/') {
        return {ValueTokenType::Operator, std::string(1, advance())};
    }

    if (c == '+' || c == '-' || c == '*' || c == '/') {
        return {ValueTokenType::Operator, std::string(1, advance())};
    }

    if (c == '.') {
        return {ValueTokenType::Dot, std::string(1, advance())};
    }

    if (c == '#') { // Selectors start with #, class selectors with . are not handled yet
        std::string value;
        value += advance(); // consume #
        while (!isAtEnd() && isalnum(peek())) {
            value += advance();
        }
        return {ValueTokenType::Selector, value};
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
