#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        Token token = nextToken();
        tokens.push_back(token);
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();

    if (isAtEnd()) {
        return {TokenType::EndOfFile, "", line};
    }

    char c = peek();

    // Handles identifiers and unquoted literals
    if (isalpha(c) || c == '_') {
        std::string value;
        value += advance();
        while (!isAtEnd() && (isalnum(peek()) || peek() == '_' || peek() == '-')) {
            value += advance();
        }
        return {TokenType::Identifier, value, line};
    }

    if (c == '"' || c == '\'') {
        return handleStringLiteral();
    }

    // Handles single-character tokens
    switch (c) {
        case '{': advance(); return {TokenType::LeftBrace, "{", line};
        case '}': advance(); return {TokenType::RightBrace, "}", line};
        case ':': advance(); return {TokenType::Colon, ":", line};
        case ';': advance(); return {TokenType::Semicolon, ";", line};
        case '=': advance(); return {TokenType::Equals, "=", line};
    }

    // Fallback for unquoted literals that don't start with a letter (e.g. numbers)
    // or for other characters. This is a simple implementation.
    if (!isspace(c)) {
        std::string value;
        value += advance();
        while (!isAtEnd() && !isspace(peek()) && peek() != '{' && peek() != '}' && peek() != ':' && peek() != ';' && peek() != '=') {
            value += advance();
        }
        return {TokenType::Identifier, value, line};
    }


    advance();
    return {TokenType::Unknown, std::string(1, c), line};
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (isspace(c)) {
            if (c == '\n') line++;
            advance();
        } else if (c == '/' && (current + 1 < source.length() && source[current+1] == '/')) {
            // Single line comment
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else if (c == '/' && (current + 1 < source.length() && source[current+1] == '*')) {
            // Multi-line comment
            advance(); // Consume /
            advance(); // Consume *
            while (!isAtEnd()) {
                if (peek() == '*' && (current + 1 < source.length() && source[current+1] == '/')) {
                    advance(); // Consume *
                    advance(); // Consume /
                    break;
                }
                if (peek() == '\n') line++;
                advance();
            }
        }
        else if (c == '#') {
             while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        }
        else {
            break;
        }
    }
}

Token Lexer::handleStringLiteral() {
    char quote_type = advance(); // Consume opening quote
    std::string value;
    while (!isAtEnd() && peek() != quote_type) {
        if (peek() == '\n') line++;
        value += advance();
    }

    if (isAtEnd()) {
        // Unterminated string
        return {TokenType::Unknown, value, line};
    }

    advance(); // consume closing quote
    return {TokenType::StringLiteral, value, line};
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return source[current - 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}
