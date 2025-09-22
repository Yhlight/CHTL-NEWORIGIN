#include "Lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        Token token = scan_token();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }
    return tokens;
}

Token Lexer::scan_token() {
    skip_whitespace_and_comments();

    if (is_at_end()) return make_token(TokenType::END_OF_FILE, "EOF");

    char c = advance();

    if (std::isdigit(c)) return number();
    if (std::isalpha(c) || c == '_') return identifier();
    if (c == '"') return string_literal();

    switch (c) {
        case '{': return make_token(TokenType::LBRACE);
        case '}': return make_token(TokenType::RBRACE);
        case ':': return make_token(TokenType::COLON);
        case ';': return make_token(TokenType::SEMICOLON);
        case '[': return make_token(TokenType::LBRACKET);
        case ']': return make_token(TokenType::RBRACKET);
        case '@': return make_token(TokenType::AT);
        case '+': return make_token(TokenType::PLUS);
        case '-': return make_token(TokenType::MINUS);
        case '*': return make_token(TokenType::STAR);
        case '/': return make_token(TokenType::SLASH);
        case '.': return make_token(TokenType::DOT);
        case '&': return make_token(TokenType::AMPERSAND);
        case '(': return make_token(TokenType::LPAREN);
        case ')': return make_token(TokenType::RPAREN);
        case '#': return make_token(TokenType::HASH);
    }

    return make_token(TokenType::UNKNOWN, std::string(1, c));
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source[current];
}

char Lexer::peek_next() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::is_at_end() {
    return current >= source.length();
}

void Lexer::skip_whitespace_and_comments() {
    while (!is_at_end()) {
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
                if (peek_next() == '/') { // Single line comment
                    while (peek() != '\n' && !is_at_end()) advance();
                } else if (peek_next() == '*') { // Block comment
                    advance();
                    advance();
                    while (!is_at_end() && (peek() != '*' || peek_next() != '/')) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!is_at_end()) {
                        advance();
                        advance();
                    }
                }
                else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::make_token(TokenType type, const std::string& value) {
    return Token(type, value, line, column - value.length());
}

Token Lexer::make_token(TokenType type) {
    return Token(type, std::string(source.substr(current - 1, 1)), line, column - 1);
}

Token Lexer::identifier() {
    int start = current - 1;
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') advance();
    std::string text = source.substr(start, current - start);
    if (text == "text") return make_token(TokenType::TEXT, text);
    if (text == "style") return make_token(TokenType::STYLE, text);
    if (text == "Template") return make_token(TokenType::TEMPLATE, text);
    if (text == "Custom") return make_token(TokenType::CUSTOM, text);
    if (text == "Import") return make_token(TokenType::IMPORT, text);
    if (text == "from") return make_token(TokenType::FROM, text);
    if (text == "Chtl") return make_token(TokenType::CHTL, text);
    return make_token(TokenType::IDENTIFIER, text);
}

Token Lexer::string_literal() {
    int start_col = column;
    int start_line = line;
    int start_current = current;
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }
    if (is_at_end()) {
        return Token(TokenType::UNKNOWN, "Unterminated string", start_line, start_col);
    }
    advance();
    std::string value = source.substr(start_current, current - start_current - 1);
    return Token(TokenType::STRING, value, start_line, start_col);
}

Token Lexer::number() {
    int start = current - 1;
    while (std::isdigit(peek())) advance();
    if (peek() == '.' && std::isdigit(peek_next())) {
        advance();
        while (std::isdigit(peek())) advance();
    }
    std::string value = source.substr(start, current - start);
    return make_token(TokenType::NUMBER, value);
}
