#include "Lexer.h"
#include <cctype>
#include <map>

namespace CHTL {

Lexer::Lexer(const std::string& input) : source(input) {}

bool Lexer::isAtEnd() {
    return position >= source.length();
}

char Lexer::peek(size_t offset) {
    if (position + offset >= source.length()) {
        return '\0';
    }
    return source[position + offset];
}

char Lexer::advance() {
    if (!isAtEnd()) {
        char current = source[position];
        position++;
        column++;
        if (current == '\n') {
            line++;
            column = 1;
        }
        return current;
    }
    return '\0';
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        switch (peek()) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            default:
                return;
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        tokens.push_back(scanToken());
    }
    tokens.push_back({TokenType::EndOfFile, "", line, column});
    return tokens;
}

Token Lexer::scanToken() {
    size_t start_char_index = position;
    char c = advance();
    switch (c) {
        case '{': return {TokenType::OpenBrace, "{", line, column, start_char_index};
        case '}': return {TokenType::CloseBrace, "}", line, column, start_char_index};
        case '(': return {TokenType::OpenParen, "(", line, column, start_char_index};
        case ')': return {TokenType::CloseParen, ")", line, column, start_char_index};
        case '[': return {TokenType::OpenBracket, "[", line, column, start_char_index};
        case ']': return {TokenType::CloseBracket, "]", line, column, start_char_index};
        case '&': return {TokenType::Ampersand, "&", line, column, start_char_index};
        case ':': return {TokenType::Colon, ":", line, column, start_char_index};
        case ';': return {TokenType::Semicolon, ";", line, column, start_char_index};
        case '=': return {TokenType::Equals, "=", line, column, start_char_index};
        case ',': return {TokenType::Comma, ",", line, column, start_char_index};
        case '.': return {TokenType::Dot, ".", line, column, start_char_index};
        case '@': return {TokenType::At, "@", line, column, start_char_index};
        case '+': return {TokenType::Plus, "+", line, column, start_char_index};
        case '-': return {TokenType::Minus, "-", line, column, start_char_index};
        case '*': return handleAsterisk();
        case '/': return handleSlash();
        case '#': return handleComment();
        case '"':
        case '\'':
            return makeStringLiteral(c);
        default:
            position--; column--;
            if (isdigit(c)) {
                return makeNumberLiteral();
            }
            if (isalpha(c) || c == '_') {
                return makeIdentifierOrKeyword();
            }
            return makeUnquotedLiteral();
    }
}

Token Lexer::makeNumberLiteral() {
    size_t start = position;
    while (isdigit(peek())) {
        advance();
    }
    if (peek() == '.' && isdigit(peek(1))) {
        advance();
        while (isdigit(peek())) {
            advance();
        }
    }
    return {TokenType::NumberLiteral, source.substr(start, position - start), line, column, start};
}

Token Lexer::makeIdentifierOrKeyword() {
    size_t start = position;
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string value = source.substr(start, position - start);

    static const std::map<std::string, TokenType> keywords = {
        {"text", TokenType::Text}, {"style", TokenType::Style}, {"script", TokenType::Script},
    };

    if (keywords.count(value)) {
        return {keywords.at(value), value, line, column, start};
    }
    return {TokenType::Identifier, value, line, column, start};
}

Token Lexer::makeStringLiteral(char quote_type) {
    size_t start = position;
    size_t start_char_index = position - 1; // Include the opening quote in the index range
    while (peek() != quote_type && !isAtEnd()) {
        advance();
    }
    if (isAtEnd()) {
        return {TokenType::Unknown, "Unterminated string", line, column, start_char_index};
    }
    advance(); // Consume the closing quote
    return {TokenType::StringLiteral, source.substr(start, position - start - 1), line, column, start_char_index};
}

Token Lexer::makeUnquotedLiteral() {
    size_t start = position;
    while (!isAtEnd()) {
        char c = peek();
        if (c == ';' || c == '{' || c == '}' || c == '\n') {
            break;
        }
        advance();
    }
    std::string value = source.substr(start, position - start);
    size_t end = value.find_last_not_of(" \t\r");
    if (std::string::npos != end) {
        value = value.substr(0, end + 1);
    }
    return {TokenType::UnquotedLiteral, value, line, column, start};
}

Token Lexer::handleSlash() {
    size_t start_char_index = position - 1;
    if (peek() == '/') {
        advance();
        size_t start = position;
        while (peek() != '\n' && !isAtEnd()) {
            advance();
        }
        return {TokenType::LineComment, source.substr(start, position - start), line, column, start_char_index};
    } else if (peek() == '*') {
        advance();
        size_t start = position;
        while (!isAtEnd() && (peek() != '*' || peek(1) != '/')) {
            advance();
        }
        if (!isAtEnd()) {
            advance(); advance();
        }
        return {TokenType::BlockComment, source.substr(start, position - start - 2), line, column, start_char_index};
    }
    return {TokenType::Slash, "/", line, column, start_char_index};
}

Token Lexer::handleAsterisk() {
    size_t start_char_index = position - 1;
    if (peek() == '*') {
        advance();
        return {TokenType::DoubleAsterisk, "**", line, column, start_char_index};
    }
    return {TokenType::Asterisk, "*", line, column, start_char_index};
}

Token Lexer::handleComment() {
    size_t start_char_index = position - 1;
    if (peek() == ' ') {
        advance();
        size_t start = position;
        while (peek() != '\n' && !isAtEnd()) {
            advance();
        }
        return {TokenType::GeneratorComment, source.substr(start, position - start), line, column, start_char_index};
    }
    return {TokenType::Hash, "#", line, column, start_char_index};
}

} // namespace CHTL
