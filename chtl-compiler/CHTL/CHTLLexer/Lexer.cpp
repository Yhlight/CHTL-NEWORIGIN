#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

char Lexer::currentChar() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

void Lexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    position++;
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

Token Lexer::identifier() {
    std::string value;
    int start_line = line;
    int start_column = column;
    while (currentChar() != '\0' && (isalnum(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }
    return {TokenType::Identifier, value, start_line, start_column};
}

Token Lexer::stringLiteral() {
    std::string value;
    int start_line = line;
    int start_column = column;
    advance(); // Consume the opening quote
    while (currentChar() != '\0' && currentChar() != '"') {
        value += currentChar();
        advance();
    }
    advance(); // Consume the closing quote
    return {TokenType::StringLiteral, value, start_line, start_column};
}

Token Lexer::getNextToken() {
    skipWhitespace();

    char current = currentChar();

    if (current == '\0') {
        return {TokenType::EndOfFile, "", line, column};
    }

    if (isalpha(current)) {
        return identifier();
    }

    if (current == '"') {
        return stringLiteral();
    }

    if (current == '{') {
        advance();
        return {TokenType::LeftBrace, "{", line, column - 1};
    }

    if (current == '}') {
        advance();
        return {TokenType::RightBrace, "}", line, column - 1};
    }

    advance();
    return {TokenType::Unknown, std::string(1, current), line, column - 1};
}