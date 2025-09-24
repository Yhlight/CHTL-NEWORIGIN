#include "CHTLLexer.h"
#include <map>

// Helper functions for character checks
static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

// Map of keywords to their token types
static std::map<std::string, TokenType> keywords = {
    {"text",    TokenType::KW_TEXT},
    {"style",   TokenType::KW_STYLE},
    {"script",  TokenType::KW_SCRIPT},
    {"Template",TokenType::KW_TEMPLATE},
    {"Custom",  TokenType::KW_CUSTOM},
    {"Origin",  TokenType::KW_ORIGIN},
    {"Import",  TokenType::KW_IMPORT},
    {"Namespace", TokenType::KW_NAMESPACE},
    {"Configuration", TokenType::KW_CONFIGURATION}
};


CHTLLexer::CHTLLexer(const std::string& source) : source(source) {
}

std::vector<Token> CHTLLexer::tokenize() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", "", line});
    return tokens;
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

char CHTLLexer::advance() {
    current++;
    return source[current - 1];
}

char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void CHTLLexer::addToken(TokenType type) {
    addToken(type, "");
}

void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, literal, line});
}

void CHTLLexer::processString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        addToken(TokenType::ILLEGAL, "Unterminated string.");
        return;
    }

    advance(); // The closing ".
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING_LITERAL, value);
}

void CHTLLexer::processIdentifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords[text];
    }
    addToken(type, text); // The literal for an identifier is the text itself
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE, "{"); break;
        case '}': addToken(TokenType::RIGHT_BRACE, "}"); break;
        case ';': addToken(TokenType::SEMICOLON, ";"); break;

        case '/':
            if (peek() == '/') { // Single-line comment
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (peek() == '*') { // Multi-line comment
                advance(); // Consume '*'
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // Consume '*'
                    advance(); // Consume '/'
                }
            } else {
                addToken(TokenType::SLASH, "/");
            }
            break;

        case '#': // Generator comment
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            addToken(TokenType::GENERATOR_COMMENT, source.substr(start + 1, current - (start + 1)));
            break;

        // Skip whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;

        case '"': processString(); break;

        default:
            if (isAlpha(c)) {
                processIdentifier();
            } else {
                addToken(TokenType::ILLEGAL, "Unexpected character.");
            }
            break;
    }
}
