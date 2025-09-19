#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

Token Lexer::nextToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return {TokenType::TOKEN_EOF, "", line};
    }

    char c = advance();

    if (isAlpha(c)) return readIdentifier();
    if (c == '"') return readString();

    switch (c) {
        case '{': return {TokenType::LBRACE, "{", line};
        case '}': return {TokenType::RBRACE, "}", line};
        case ':': return {TokenType::COLON, ":", line};
        case ';': return {TokenType::SEMICOLON, ";", line};
        // Handle comments later
    }

    return {TokenType::TOKEN_ERROR, "Unexpected character.", line};
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            // For now, we will not handle comments, but they would be skipped here.
            /*
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            */
            default:
                return;
        }
    }
}

Token Lexer::readIdentifier() {
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    std::string value = source.substr(start, current - start);
    return {TokenType::IDENTIFIER, value, line};
}

Token Lexer::readString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        return {TokenType::TOKEN_ERROR, "Unterminated string.", line};
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    std::string value = source.substr(start + 1, current - start - 2);
    return {TokenType::STRING, value, line};
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::advance() {
    current++;
    return source[current - 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}
