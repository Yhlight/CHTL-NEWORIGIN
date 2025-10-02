#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char CHTLLexer::peek() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char CHTLLexer::advance() {
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

void CHTLLexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

Token CHTLLexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token{type, lexeme, line, column - (int)lexeme.length()};
}

Token CHTLLexer::errorToken(const std::string& message) {
    return Token{TokenType::TOKEN_UNKNOWN, message, line, column};
}

Token CHTLLexer::getNextToken() {
    skipWhitespace();
    char c = advance();

    if (c == '\0') {
        return makeToken(TokenType::TOKEN_EOF, "");
    }

    switch (c) {
        case '{': return makeToken(TokenType::TOKEN_LBRACE, "{");
        case '}': return makeToken(TokenType::TOKEN_RBRACE, "}");
        case '(': return makeToken(TokenType::TOKEN_LPAREN, "(");
        case ')': return makeToken(TokenType::TOKEN_RPAREN, ")");
        case '[': return makeToken(TokenType::TOKEN_LBRACKET, "[");
        case ']': return makeToken(TokenType::TOKEN_RBRACKET, "]");
        case ':': return makeToken(TokenType::TOKEN_COLON, ":");
        case ';': return makeToken(TokenType::TOKEN_SEMICOLON, ";");
        case ',': return makeToken(TokenType::TOKEN_COMMA, ",");
        case '.': return makeToken(TokenType::TOKEN_DOT, ".");
        case '=': return makeToken(TokenType::TOKEN_ASSIGN, "=");
        case '+': return makeToken(TokenType::TOKEN_PLUS, "+");
        case '-': return makeToken(TokenType::TOKEN_MINUS, "-");
        case '*': return makeToken(TokenType::TOKEN_MULTIPLY, "*");
        case '?': return makeToken(TokenType::TOKEN_QUESTION, "?");

        case '/':
            if (peek() == '/') {
                advance();
                return singleLineComment();
            } else if (peek() == '*') {
                advance();
                return multiLineComment();
            }
            return makeToken(TokenType::TOKEN_DIVIDE, "/");

        case '#':
            return generatorComment();

        case '"':
            return stringLiteral();

        default:
            if (isalpha(c) || c == '_') {
                position--;
                column--;
                return identifier();
            }
            if (isdigit(c)) {
                 position--;
                 column--;
                return number();
            }
            return errorToken("Unexpected character");
    }
}

Token CHTLLexer::identifier() {
    std::string lexeme;
    while (isalnum(peek()) || peek() == '_') {
        lexeme += advance();
    }

    if (lexeme == "text") return makeToken(TokenType::TOKEN_TEXT, lexeme);
    if (lexeme == "style") return makeToken(TokenType::TOKEN_STYLE, lexeme);
    if (lexeme == "script") return makeToken(TokenType::TOKEN_SCRIPT, lexeme);

    return makeToken(TokenType::TOKEN_IDENTIFIER, lexeme);
}

Token CHTLLexer::number() {
    std::string lexeme;
    while (isdigit(peek()) || peek() == '.') {
        lexeme += advance();
    }
    return makeToken(TokenType::TOKEN_NUMERIC_LITERAL, lexeme);
}

Token CHTLLexer::stringLiteral() {
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

Token CHTLLexer::singleLineComment() {
    std::string lexeme;
    while (peek() != '\n' && peek() != '\0') {
        lexeme += advance();
    }
    return makeToken(TokenType::TOKEN_SINGLE_LINE_COMMENT, lexeme);
}

Token CHTLLexer::multiLineComment() {
    std::string lexeme;
    while (position + 1 < source.length() && (peek() != '*' || source[position + 1] != '/')) {
        lexeme += advance();
    }

    if (position + 1 < source.length() && peek() == '*' && source[position + 1] == '/') {
        advance(); // consume *
        advance(); // consume /
    } else {
        return errorToken("Unterminated multi-line comment");
    }
    return makeToken(TokenType::TOKEN_MULTI_LINE_COMMENT, lexeme);
}

Token CHTLLexer::generatorComment() {
    std::string lexeme;
    while (peek() != '\n' && peek() != '\0') {
        lexeme += advance();
    }
    return makeToken(TokenType::TOKEN_GENERATOR_COMMENT, lexeme);
}

}