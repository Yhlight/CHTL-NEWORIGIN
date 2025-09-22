#include "Lexer.h"
#include <iostream>
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scan_tokens() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        start = current;
        Token token = scan_token();
        if (token.type != TokenType::UNKNOWN) {
             tokens.push_back(token);
        }
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

Token Lexer::scan_token() {
    skip_whitespace_and_comments();
    start = current;

    if (is_at_end()) return make_token(TokenType::END_OF_FILE);

    char c = advance();

    if (std::isalpha(c) || c == '_') return identifier();
    if (std::isdigit(c)) return number();

    switch (c) {
        case '(': return make_token(TokenType::LEFT_PAREN);
        case ')': return make_token(TokenType::RIGHT_PAREN);
        case '{': return make_token(TokenType::LEFT_BRACE);
        case '}': return make_token(TokenType::RIGHT_BRACE);
        case ':': return make_token(TokenType::COLON);
        case '=': return make_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case ';': return make_token(TokenType::SEMICOLON);
        case '?': return make_token(TokenType::QUESTION);
        case '+': return make_token(TokenType::PLUS);
        case '-': return make_token(TokenType::MINUS);
        case '/': return make_token(TokenType::SLASH);
        case '%': return make_token(TokenType::PERCENT);
        case '*': return make_token(match('*') ? TokenType::STAR_STAR : TokenType::STAR);
        case '>': return make_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '<': return make_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '!': return make_token(match('=') ? TokenType::BANG_EQUAL : TokenType::UNKNOWN);
        case '&': return make_token(match('&') ? TokenType::LOGICAL_AND : TokenType::AMPERSAND);
        case '|': return make_token(match('|') ? TokenType::LOGICAL_OR : TokenType::UNKNOWN);
        case '.': return make_token(TokenType::DOT);
        case '#':
            if (peek() == ' ') {
                while (peek() != '\n' && !is_at_end()) advance();
                return make_token(TokenType::HASH_COMMENT);
            } else {
                return make_token(TokenType::HASH);
            }
        case '"': return string_literal('"');
        case '\'': return string_literal('\'');
    }

    return error_token("Unexpected character.");
}

void Lexer::skip_whitespace_and_comments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            case '/':
                if (peek_next() == '/') {
                    while (peek() != '\n' && !is_at_end()) advance();
                } else if (peek_next() == '*') {
                    advance(); advance();
                    while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                        if (peek() == '\n') { line++; column = 1; }
                        advance();
                    }
                    if (!is_at_end()) { advance(); advance(); }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

bool Lexer::is_at_end() const {
    return current >= source.length();
}

char Lexer::advance() {
    if (!is_at_end()) {
        if (source[current] != '\n') { column++; }
        current++;
    }
    return source[current - 1];
}

char Lexer::peek() const {
    if (is_at_end()) return '\0';
    return source[current];
}

char Lexer::peek_next() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (is_at_end()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

Token Lexer::make_token(TokenType type) const {
    return {type, source.substr(start, current - start), line, (int)(column - (current - start))};
}

Token Lexer::error_token(const std::string& message) const {
     return {TokenType::UNKNOWN, message, line, column};
}

Token Lexer::string_literal(char quote_type) {
    start = current;
    while (peek() != quote_type && !is_at_end()) {
        if (peek() == '\n') { line++; column = 1; }
        advance();
    }
    if (is_at_end()) return error_token("Unterminated string.");
    std::string value = source.substr(start, current - start);
    advance();
    return {TokenType::STRING, value, line, (int)(column - value.length() - 1)};
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string text = source.substr(start, current - start);
    if (text == "text") return make_token(TokenType::TEXT_KEYWORD);
    return make_token(TokenType::IDENTIFIER);
}

Token Lexer::number() {
    while (std::isdigit(peek())) advance();
    if (peek() == '.' && std::isdigit(peek_next())) {
        advance();
        while (std::isdigit(peek())) advance();
    }
    return make_token(TokenType::NUMBER);
}
