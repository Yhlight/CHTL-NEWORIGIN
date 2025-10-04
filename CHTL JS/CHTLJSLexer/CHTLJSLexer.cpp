#include "CHTLJSLexer.h"
#include <cctype>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source(source), position(0) {}

char CHTLJSLexer::peek() {
    if (position >= source.length()) return '\0';
    return source[position];
}

char CHTLJSLexer::advance() {
    if (position >= source.length()) return '\0';
    return source[position++];
}

void CHTLJSLexer::skipWhitespace() {
    while (position < source.length() && isspace(source[position])) {
        position++;
    }
}

Token CHTLJSLexer::getNextToken() {
    skipWhitespace();

    if (position >= source.length()) {
        return {TokenType::TOKEN_EOF, ""};
    }

    char current = peek();

    if (current == '{' && position + 1 < source.length() && source[position + 1] == '{') {
        advance();
        advance();
        return {TokenType::TOKEN_DOUBLE_LBRACE, "{{"};
    }

    if (current == '}' && position + 1 < source.length() && source[position + 1] == '}') {
        advance();
        advance();
        return {TokenType::TOKEN_DOUBLE_RBRACE, "}}"};
    }

    if (current == '.') {
        return {TokenType::TOKEN_DOT, std::string(1, advance())};
    }
    if (current == '#') {
        return {TokenType::TOKEN_HASH, std::string(1, advance())};
    }
    if (current == '[') {
        return {TokenType::TOKEN_LBRACKET, std::string(1, advance())};
    }
    if (current == ']') {
        return {TokenType::TOKEN_RBRACKET, std::string(1, advance())};
    }
    if (current == '=') {
        return {TokenType::TOKEN_ASSIGN, std::string(1, advance())};
    }
    if (current == ';') {
        return {TokenType::TOKEN_SEMICOLON, std::string(1, advance())};
    }

    if (current == '"' || current == '\'') {
        char quote_type = current;
        advance(); // consume opening quote
        std::string lexeme;
        while (position < source.length() && source[position] != quote_type) {
            lexeme += advance();
        }
        advance(); // consume closing quote
        return {TokenType::TOKEN_STRING_LITERAL, lexeme};
    }

    if (isalpha(current) || current == '_') {
        std::string lexeme;
        while (position < source.length() && (isalnum(source[position]) || source[position] == '_')) {
            lexeme += advance();
        }
        return {TokenType::TOKEN_IDENTIFIER, lexeme};
    }

    if (isdigit(current)) {
        std::string lexeme;
        while (position < source.length() && isdigit(source[position])) {
            lexeme += advance();
        }
        return {TokenType::TOKEN_NUMERIC_LITERAL, lexeme};
    }

    return {TokenType::TOKEN_UNKNOWN, std::string(1, advance())};
}

}