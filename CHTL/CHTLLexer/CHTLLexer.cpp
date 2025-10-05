#include "CHTLLexer.h"
#include <cctype>
#include <cstring> // For strchr

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source, std::shared_ptr<ConfigurationManager> configManager)
    : source(source), configManager(configManager), position(0), line(1), column(1) {}

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
    char c = peek();

    if (c == '\0') {
        return makeToken(TokenType::TOKEN_EOF, "");
    }

    switch (c) {
        case '@': advance(); return makeToken(TokenType::TOKEN_AT, "@");
        case '{': advance(); return makeToken(TokenType::TOKEN_LBRACE, "{");
        case '}': advance(); return makeToken(TokenType::TOKEN_RBRACE, "}");
        case '(': advance(); return makeToken(TokenType::TOKEN_LPAREN, "(");
        case ')': advance(); return makeToken(TokenType::TOKEN_RPAREN, ")");
        case '[': advance(); return makeToken(TokenType::TOKEN_LBRACKET, "[");
        case ']': advance(); return makeToken(TokenType::TOKEN_RBRACKET, "]");
        case ':': advance(); return makeToken(TokenType::TOKEN_COLON, ":");
        case ';': advance(); return makeToken(TokenType::TOKEN_SEMICOLON, ";");
        case ',': advance(); return makeToken(TokenType::TOKEN_COMMA, ",");
        case '.': advance(); return makeToken(TokenType::TOKEN_DOT, ".");
        case '=': advance(); return makeToken(TokenType::TOKEN_ASSIGN, "=");
        case '+': advance(); return makeToken(TokenType::TOKEN_PLUS, "+");
        case '-': advance(); return makeToken(TokenType::TOKEN_MINUS, "-");
        case '*':
            advance();
            if (peek() == '*') {
                advance();
                return makeToken(TokenType::TOKEN_POWER, "**");
            }
            return makeToken(TokenType::TOKEN_MULTIPLY, "*");
        case '%': advance(); return makeToken(TokenType::TOKEN_MODULO, "%");
        case '?': advance(); return makeToken(TokenType::TOKEN_QUESTION, "?");

        case '/':
            advance();
            if (peek() == '/') {
                advance();
                return singleLineComment();
            } else if (peek() == '*') {
                advance();
                return multiLineComment();
            }
            return makeToken(TokenType::TOKEN_DIVIDE, "/");

        case '#':
            advance();
            return generatorComment();

        case '"':
        case '\'':
            advance();
            return stringLiteral(c);

        default:
            return lexIdentifierOrLiteral();
    }
}

Token CHTLLexer::lexIdentifierOrLiteral() {
    std::string lexeme;
    const char* delimiters = "{}()[]:;,.=?# \t\n\r";
    while (peek() != '\0' && strchr(delimiters, peek()) == nullptr && !isspace(peek())) {
        lexeme += advance();
    }

    if (lexeme.empty()) {
        return errorToken("Unexpected character");
    }

    // Check for keywords using the configuration manager
    if (configManager) {
        if (lexeme == configManager->getKeyword("text")) return makeToken(TokenType::TOKEN_TEXT, lexeme);
        if (lexeme == configManager->getKeyword("style")) return makeToken(TokenType::TOKEN_STYLE, lexeme);
        if (lexeme == configManager->getKeyword("script")) return makeToken(TokenType::TOKEN_SCRIPT, lexeme);
        if (lexeme == configManager->getKeyword("use")) return makeToken(TokenType::TOKEN_KEYWORD_USE, lexeme);
        if (lexeme == configManager->getKeyword("html5")) return makeToken(TokenType::TOKEN_KEYWORD_HTML5, lexeme);
        if (lexeme == configManager->getKeyword("from")) return makeToken(TokenType::TOKEN_KEYWORD_FROM, lexeme);
        if (lexeme == configManager->getKeyword("if")) return makeToken(TokenType::TOKEN_IF, lexeme);
        if (lexeme == configManager->getKeyword("else")) return makeToken(TokenType::TOKEN_ELSE, lexeme);
        if (lexeme == configManager->getKeyword("except")) return makeToken(TokenType::TOKEN_EXCEPT, lexeme);
        if (lexeme == configManager->getKeyword("delete")) return makeToken(TokenType::TOKEN_DELETE, lexeme);
        if (lexeme == configManager->getKeyword("insert")) return makeToken(TokenType::TOKEN_INSERT, lexeme);
    }

    // Check if it's a number (potentially with a unit)
    if (isdigit(lexeme[0]) || (lexeme.length() > 1 && lexeme[0] == '.' && isdigit(lexeme[1]))) {
        return makeToken(TokenType::TOKEN_NUMERIC_LITERAL, lexeme);
    }

    // Check if it's a valid identifier (starts with a letter or underscore)
    if (isalpha(lexeme[0]) || lexeme[0] == '_') {
        bool isIdentifier = true;
        for (char c : lexeme) {
            if (!isalnum(c) && c != '_') {
                isIdentifier = false;
                break;
            }
        }
        if (isIdentifier) {
            return makeToken(TokenType::TOKEN_IDENTIFIER, lexeme);
        }
    }

    // Otherwise, it's an unquoted literal
    return makeToken(TokenType::TOKEN_UNQUOTED_LITERAL, lexeme);
}

Token CHTLLexer::stringLiteral(char quoteType) {
    std::string lexeme;
    while (peek() != quoteType && peek() != '\0') {
        lexeme += advance();
    }
    if (peek() == quoteType) {
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