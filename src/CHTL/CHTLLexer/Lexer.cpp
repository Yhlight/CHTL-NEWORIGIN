#include "Lexer.h"
#include <cctype>
#include <map>
#include <string>

namespace CHTL {

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token token = scanToken();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }
    return tokens;
}

Token Lexer::scanToken() {
    skipWhitespace();
    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE);

    char c = advance();

    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '[': return makeToken(TokenType::LEFT_BRACKET);
        case ']': return makeToken(TokenType::RIGHT_BRACKET);
        case ':': return makeToken(TokenType::COLON);
        case ';': return makeToken(TokenType::SEMICOLON);
        case '=': return makeToken(TokenType::EQUALS);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case '?': return makeToken(TokenType::QUESTION);
        case '@': return makeToken(TokenType::AT_SYMBOL);
        case '+': return makeToken(TokenType::PLUS);
        case '-': return makeToken(TokenType::MINUS);
        case '%': return makeToken(TokenType::PERCENT);
        case '&':
            return match('&') ? makeToken(TokenType::LOGICAL_AND) : errorToken("Unexpected character '&'. Did you mean '&&'?");
        case '|':
            return match('|') ? makeToken(TokenType::LOGICAL_OR) : errorToken("Unexpected character '|'. Did you mean '||'?");
        case '*':
            return match('*') ? makeToken(TokenType::POWER) : makeToken(TokenType::STAR);
        case '/':
            if (match('/') || match('*')) {
                current--;
                return scanComment();
            }
            return makeToken(TokenType::SLASH);
        case '#':
            if (peek() == ' ') {
                return scanComment();
            }
            break;
        case '"':
        case '\'':
            return scanStringLiteral(c);
        default:
            if (std::isalpha(c) || c == '_') {
                return scanIdentifier();
            }
            if (std::isdigit(c)) {
                return scanNumberLiteral();
            }
    }
    return errorToken("Unexpected character.");
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                line++;
                column = 1;
                break;
            default:
                return;
        }
    }
}

Token Lexer::makeToken(TokenType type) {
    return {type, source.substr(current - 1, 1), line, column};
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
     return {type, lexeme, line, column};
}

Token Lexer::errorToken(const std::string& message) {
    return {TokenType::UNKNOWN, message, line, column};
}

TokenType Lexer::identifierType(const std::string& identifier) {
    static const std::map<std::string, TokenType> keywords = {
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"script", TokenType::KEYWORD_SCRIPT},
        {"inherit", TokenType::KEYWORD_INHERIT},
        {"delete", TokenType::KEYWORD_DELETE},
        {"insert", TokenType::KEYWORD_INSERT},
        {"after", TokenType::KEYWORD_AFTER},
        {"before", TokenType::KEYWORD_BEFORE},
        {"replace", TokenType::KEYWORD_REPLACE},
        {"from", TokenType::KEYWORD_FROM},
        {"as", TokenType::KEYWORD_AS},
        {"except", TokenType::KEYWORD_EXCEPT},
        {"use", TokenType::KEYWORD_USE},
        {"html5", TokenType::KEYWORD_HTML5},
        {"Custom", TokenType::KEYWORD_CUSTOM},
        {"Template", TokenType::KEYWORD_TEMPLATE},
        {"Origin", TokenType::KEYWORD_ORIGIN},
        {"Import", TokenType::KEYWORD_IMPORT},
        {"Namespace", TokenType::KEYWORD_NAMESPACE},
        {"Configuration", TokenType::KEYWORD_CONFIGURATION},
        {"Info", TokenType::KEYWORD_INFO},
        {"Export", TokenType::KEYWORD_EXPORT},
    };
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

Token Lexer::scanIdentifier() {
    size_t start = current - 1;
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string text = source.substr(start, current - start);
    return makeToken(identifierType(text), text);
}

Token Lexer::scanStringLiteral(char quoteType) {
    size_t start = current;
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        if (peek() == '\\' && peekNext() == quoteType) {
            advance();
        }
        advance();
    }
    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }
    advance(); // Consume the closing quote
    std::string value = source.substr(start, current - start - 1);
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::scanNumberLiteral() {
    size_t start = current - 1;
    while (std::isdigit(peek())) {
        advance();
    }
    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }
    std::string value = source.substr(start, current - start);
    return makeToken(TokenType::NUMBER_LITERAL, value);
}

Token Lexer::scanComment() {
    char previous = source[current - 2];
    if (previous == '/') {
        size_t start = current;
        while (peek() != '\n' && !isAtEnd()) {
            advance();
        }
        std::string value = source.substr(start, current - start);
        return makeToken(TokenType::LINE_COMMENT, value);
    }
    if (previous == '*') {
        size_t start = current;
        while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
            if (peek() == '\n') {
                line++;
                column = 1;
            }
            advance();
        }
        if (isAtEnd()) {
            return errorToken("Unterminated block comment.");
        }
        std::string value = source.substr(start, current - start);
        advance();
        advance();
        return makeToken(TokenType::BLOCK_COMMENT, value);
    }
    if (source[current-1] == '#') {
        advance(); // consume the space
        size_t start = current;
        while (peek() != '\n' && !isAtEnd()) {
            advance();
        }
        std::string value = source.substr(start, current - start);
        return makeToken(TokenType::GENERATOR_COMMENT, value);
    }
    return errorToken("Unrecognized comment starter.");
}

} // namespace CHTL
