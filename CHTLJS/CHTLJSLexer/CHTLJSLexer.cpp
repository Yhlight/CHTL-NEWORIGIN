#include "CHTLJSLexer.h"
#include <cctype>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char CHTLJSLexer::peek() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLJSLexer::advance() {
    if (position >= source.length()) {
        return '\0';
    }
    char currentChar = source[position];
    position++;
    column++;
    if (currentChar == '\n') {
        line++;
        column = 1;
    }
    return currentChar;
}

void CHTLJSLexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

Token CHTLJSLexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token{type, lexeme, line, column - (int)lexeme.length()};
}

Token CHTLJSLexer::errorToken(const std::string& message) {
    return Token{TokenType::TOKEN_UNKNOWN, message, line, column};
}

Token CHTLJSLexer::getNextToken() {
    skipWhitespace();
    char c = peek();

    if (c == '\0') {
        return makeToken(TokenType::TOKEN_EOF, "");
    }

    switch (c) {
        case '{': advance(); return makeToken(TokenType::TOKEN_LBRACE, "{");
        case '}': advance(); return makeToken(TokenType::TOKEN_RBRACE, "}");
        case ':': advance(); return makeToken(TokenType::TOKEN_COLON, ":");
        case ',': advance(); return makeToken(TokenType::TOKEN_COMMA, ",");
        case '"':
            advance();
            return stringLiteral();
        default:
            return lexIdentifier();
    }
}

Token CHTLJSLexer::lexIdentifier() {
    std::string lexeme;
    while (!isspace(peek()) && peek() != '\0' && peek() != '{' && peek() != '}' && peek() != ':' && peek() != ',') {
        lexeme += advance();
    }

    if (lexeme == "ScriptLoader" || lexeme == "load") {
        return makeToken(TokenType::TOKEN_IDENTIFIER, lexeme);
    }

    return makeToken(TokenType::TOKEN_UNQUOTED_LITERAL, lexeme);
}

Token CHTLJSLexer::stringLiteral() {
    std::string lexeme;
    while (peek() != '"' && peek() != '\0') {
        lexeme += advance();
    }
    if (peek() == '"') {
        advance(); // consume the closing quote
    } else {
        return errorToken("Unterminated string literal");
    }
    return makeToken(TokenType::TOKEN_STRING_LITERAL, lexeme);
}

}