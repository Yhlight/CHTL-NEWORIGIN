#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source) : source(source) {}

char Lexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespaceAndComments() {
    while (position < source.length()) {
        char c = currentChar();
        if (std::isspace(c)) {
            advance();
            continue;
        }
        if (c == '/' && position + 1 < source.length() && source[position + 1] == '/') {
            while (currentChar() != '\n' && currentChar() != '\0') {
                advance();
            }
            continue;
        }
        if (c == '/' && position + 1 < source.length() && source[position + 1] == '*') {
            advance(); advance();
            while (position + 1 < source.length() && !(currentChar() == '*' && source[position + 1] == '/')) {
                advance();
            }
            if (position + 1 < source.length()) {
                advance(); advance();
            }
            continue;
        }
        break;
    }
}

Token Lexer::makeIdentifier() {
    size_t start_pos = position;
    int start_line = line;
    int start_col = column;
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        advance();
    }
    std::string text = source.substr(start_pos, position - start_pos);

    if (text == "text") return {TokenType::TextKeyword, text, start_line, start_col};
    if (text == "inherit") return {TokenType::InheritKeyword, text, start_line, start_col};

    return {TokenType::Identifier, text, start_line, start_col};
}

Token Lexer::makeString() {
    int start_line = line;
    int start_col = column;
    char quote_type = currentChar();
    advance();
    std::string value;
    while (currentChar() != quote_type && currentChar() != '\0') {
        value += currentChar();
        advance();
    }
    if (currentChar() == quote_type) {
        advance();
    } else {
        return {TokenType::Unknown, "Unterminated String", start_line, start_col};
    }
    return {TokenType::StringLiteral, value, start_line, start_col};
}

Token Lexer::getNextToken() {
    skipWhitespaceAndComments();
    int start_line = line;
    int start_col = column;
    char c = currentChar();

    if (c == '\0') return {TokenType::EndOfFile, "", start_line, start_col};
    if (std::isdigit(c)) {
        size_t start_pos = position;
        while (std::isalnum(currentChar()) || currentChar() == '%' || currentChar() == '.') {
            advance();
        }
        std::string text = source.substr(start_pos, position - start_pos);
        return {TokenType::Identifier, text, start_line, start_col};
    }
    if (std::isalpha(c) || c == '_') return makeIdentifier();
    if (c == '"' || c == '\'') return makeString();
    if (c == '{') { advance(); return {TokenType::LeftBrace, "{", start_line, start_col}; }
    if (c == '}') { advance(); return {TokenType::RightBrace, "}", start_line, start_col}; }
    if (c == ';') { advance(); return {TokenType::Semicolon, ";", start_line, start_col}; }
    if (c == ':') { advance(); return {TokenType::Colon, ":", start_line, start_col}; }
    if (c == '=') { advance(); return {TokenType::Equals, "=", start_line, start_col}; }
    if (c == '.') { advance(); return {TokenType::Dot, ".", start_line, start_col}; }
    if (c == '#') { advance(); return {TokenType::Hash, "#", start_line, start_col}; }
    if (c == '&') { advance(); return {TokenType::Ampersand, "&", start_line, start_col}; }
    if (c == '[') { advance(); return {TokenType::LeftBracket, "[", start_line, start_col}; }
    if (c == ']') { advance(); return {TokenType::RightBracket, "]", start_line, start_col}; }
    if (c == '@') { advance(); return {TokenType::At, "@", start_line, start_col}; }

    advance();
    return {TokenType::Unknown, std::string(1, c), start_line, start_col};
}

} // namespace CHTL
