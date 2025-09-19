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
        case ':':
        case '=':
            return {TokenType::COLON, ":", line};
        case ';': return {TokenType::SEMICOLON, ";", line};
        case ',': return {TokenType::COMMA, ",", line};
        case '#':
            if (peek() == ' ') {
                // Consume the space
                advance();
                // The comment goes to the end of the line.
                start = current;
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                return {TokenType::HASH_COMMENT, source.substr(start, current - start), line};
            }
            break; // Fall through to error if not '# '
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
                    // A single-line comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (peekNext() == '*') {
                    // A block comment.
                    advance(); // consume '/'
                    advance(); // consume '*'
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
                    }
                    if (isAtEnd()) {
                        // Unclosed comment. The lexer will probably hit EOF
                        // and the parser will complain. This is okay for now.
                        return;
                    }
                    advance(); // consume '*'
                    advance(); // consume '/'
                } else {
                    return; // It's a division operator, not a comment.
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

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}
