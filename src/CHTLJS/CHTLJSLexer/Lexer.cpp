#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTLJS {

// Keyword map for CHTL JS
const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"Listen", TokenType::Listen},
    {"Delegate", TokenType::Delegate},
    {"Animate", TokenType::Animate},
    {"Vir", TokenType::Vir},
    {"Router", TokenType::Router}
};

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char Lexer::peek() {
    if (position >= source.length()) return '\0';
    return source[position];
}

char Lexer::advance() {
    if (position >= source.length()) return '\0';
    char current = source[position];
    position++;
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current;
}

void Lexer::skipWhitespace() {
    while (position < source.length() && isspace(peek())) {
        advance();
    }
}

Token Lexer::identifier() {
    std::string value;
    int startCol = column;
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }
    auto it = KEYWORDS.find(value);
    if (it != KEYWORDS.end()) {
        return {it->second, value, line, startCol};
    }
    return {TokenType::Identifier, value, line, startCol};
}

Token Lexer::stringLiteral() {
    std::string value;
    int startCol = column;
    advance(); // Consume opening quote
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') { // Handle escape sequences
            advance();
        }
        value += advance();
    }
    if (peek() == '"') {
        advance(); // Consume closing quote
    }
    return {TokenType::String, value, line, startCol};
}

Token Lexer::number() {
    std::string value;
    int startCol = column;
    while (isdigit(peek()) || peek() == '.') {
        value += advance();
    }
    return {TokenType::Number, value, line, startCol};
}


Token Lexer::getNextToken() {
    skipWhitespace();
    if (position >= source.length()) return {TokenType::EndOfFile, "", line, column};

    char current = peek();
    int startCol = column;

    if (isalpha(current) || current == '_') return identifier();
    if (isdigit(current)) return number();
    if (current == '"') return stringLiteral();

    switch (current) {
        case '-':
            if (position + 1 < source.length() && source[position + 1] == '>') {
                advance(); advance();
                return {TokenType::Arrow, "->", line, startCol};
            }
            break;
        case '{': advance(); return {TokenType::OpenBrace, "{", line, startCol};
        case '}': advance(); return {TokenType::CloseBrace, "}", line, startCol};
        case '(': advance(); return {TokenType::OpenParen, "(", line, startCol};
        case ')': advance(); return {TokenType::CloseParen, ")", line, startCol};
        case ':': advance(); return {TokenType::Colon, ":", line, startCol};
        case ',': advance(); return {TokenType::Comma, ",", line, startCol};
        case ';': advance(); return {TokenType::Semicolon, ";", line, startCol};
        case '<': // Placeholder for future operators
            break;
    }

    // Fallback for unexpected characters
    advance();
    return {TokenType::Unexpected, std::string(1, current), line, startCol};
}

} // namespace CHTLJS