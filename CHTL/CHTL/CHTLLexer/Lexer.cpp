#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token Lexer::readNumber() {
    while (isDigit(peek())) {
        advance();
    }
    // Does not yet support floating point numbers.
    return {TokenType::NUMBER, source.substr(start, current - start), line};
}

Token Lexer::nextToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return {TokenType::TOKEN_EOF, "", line};
    }

    char c = advance();

    if (isAlpha(c) || c == '-') return readIdentifier();
    if (isDigit(c)) return readNumber();
    if (c == '"') return readString();

    switch (c) {
        case '{': return {TokenType::LBRACE, "{", line};
        case '}': return {TokenType::RBRACE, "}", line};
        case ':':
        case '=':
            return {TokenType::COLON, ":", line};
        case ';': return {TokenType::SEMICOLON, ";", line};
        case ',': return {TokenType::COMMA, ",", line};
        case '.': return {TokenType::DOT, ".", line};
        case '#':
            // Disambiguate between #id selector and # comment
            if (peek() == ' ') {
                // It's a generator comment
                advance();
                start = current;
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                return {TokenType::HASH_COMMENT, source.substr(start, current - start), line};
            } else {
                // It's an ID selector
                return {TokenType::HASH, "#", line};
            }
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
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (peekNext() == '*') {
                    advance();
                    advance();
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (isAtEnd()) {
                        return;
                    }
                    advance();
                    advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::readIdentifier() {
    while (isAlpha(peek()) || isDigit(peek()) || peek() == '-') {
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

    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    return {TokenType::STRING, value, line};
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
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
