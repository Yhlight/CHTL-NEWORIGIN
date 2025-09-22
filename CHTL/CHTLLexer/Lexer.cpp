#include "Lexer.h"
#include <iostream>

// Helper functions
namespace {
    bool is_digit(char c) { return c >= '0' && c <= '9'; }
    bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
    bool is_alnum(char c) { return is_alpha(c) || is_digit(c); }
}

Lexer::Lexer(const std::string& source, const CHTLContext& context)
    : source(source), context(context) {}

std::vector<Token> Lexer::scan_tokens() {
    while (!is_at_end()) {
        start = current;
        scan_token();
    }
    tokens.push_back({TokenType::TOKEN_EOF, "", line, col});
    return tokens;
}

void Lexer::scan_token() {
    skip_whitespace_and_comments();
    start = current;
    if (is_at_end()) return;

    char c = advance();

    if (is_alpha(c)) {
        handle_identifier();
        return;
    }
    if (is_digit(c)) {
        handle_number();
        return;
    }

    switch (c) {
        case '(': add_token(TokenType::TOKEN_LPAREN); break;
        case ')': add_token(TokenType::TOKEN_RPAREN); break;
        case '{': add_token(TokenType::TOKEN_LBRACE); break;
        case '}': add_token(TokenType::TOKEN_RBRACE); break;
        case '[': add_token(TokenType::TOKEN_LBRACK); break;
        case ']': add_token(TokenType::TOKEN_RBRACK); break;
        case ';': add_token(TokenType::TOKEN_SEMICOLON); break;
        case ':': add_token(TokenType::TOKEN_COLON); break;
        case ',': add_token(TokenType::TOKEN_COMMA); break;
        case '.': add_token(TokenType::TOKEN_DOT); break;
        case '-': add_token(TokenType::TOKEN_MINUS); break;
        case '+': add_token(TokenType::TOKEN_PLUS); break;
        case '%': add_token(TokenType::TOKEN_PERCENT); break;
        case '?': add_token(TokenType::TOKEN_QUESTION); break;
        case '$': add_token(TokenType::TOKEN_DOLLAR); break;

        case '@':
            if (is_alpha(peek())) {
                while (is_alnum(peek())) advance();
                add_token(TokenType::TOKEN_AT_IDENTIFIER);
            } else {
                add_token(TokenType::TOKEN_AT);
            }
            break;

        case '*': add_token(match('*') ? TokenType::TOKEN_STAR_STAR : TokenType::TOKEN_STAR); break;
        case '=': add_token(TokenType::TOKEN_EQUAL); break;
        case '#': handle_hash_comment(); break;
        case '&': add_token(match('&') ? TokenType::TOKEN_LOGICAL_AND : TokenType::TOKEN_AMPERSAND); break;
        case '|': if (match('|')) { add_token(TokenType::TOKEN_LOGICAL_OR); } break;
        case '>': add_token(TokenType::TOKEN_GT); break;
        case '<': add_token(TokenType::TOKEN_LT); break;
        case '"': handle_string('"'); break;
        case '\'': handle_string('\''); break;
        case '/': add_token(TokenType::TOKEN_SLASH); break;

        default:
            if (is_alpha(c)) {
                handle_identifier();
            }
            break;
    }
}

void Lexer::handle_identifier() {
    while (is_alnum(peek()) || peek() == '-') advance();
    std::string text = source.substr(start, current - start);

    // Look up the identifier in the context's keyword map.
    auto it = context.keyword_to_token_map.find(text);
    TokenType type = (it != context.keyword_to_token_map.end()) ? it->second : TokenType::TOKEN_IDENTIFIER;
    add_token(type);
}

// --- Other private methods are unchanged ---

bool Lexer::is_at_end() { return current >= source.length(); }
char Lexer::advance() {
    current++;
    col++;
    return source[current - 1];
}
char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source[current];
}
char Lexer::peek_next() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}
bool Lexer::match(char expected) {
    if (is_at_end() || source[current] != expected) return false;
    current++;
    col++;
    return true;
}
void Lexer::add_token(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, col - (int)text.length()});
}
void Lexer::add_token(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line, col - (int)literal.length()});
}
void Lexer::skip_whitespace_and_comments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ': case '\r': case '\t': advance(); break;
            case '\n': line++; col = 1; advance(); break;
            case '/':
                if (peek_next() == '/') { while (peek() != '\n' && !is_at_end()) advance(); }
                else if (peek_next() == '*') {
                    advance(); advance();
                    while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                        if (peek() == '\n') { line++; col = 1; }
                        advance();
                    }
                    if (!is_at_end()) { advance(); advance(); }
                } else { return; }
                break;
            default: return;
        }
    }
}
void Lexer::handle_string(char quote_type) {
    while (peek() != quote_type && !is_at_end()) {
        if (peek() == '\n') { line++; col = 1; }
        advance();
    }
    if (is_at_end()) return;
    advance();
    std::string value = source.substr(start + 1, current - start - 2);
    add_token(quote_type == '"' ? TokenType::TOKEN_STRING_DOUBLE : TokenType::TOKEN_STRING_SINGLE, value);
}
void Lexer::handle_number() {
    while (is_digit(peek())) advance();
    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek())) advance();
    }
    add_token(TokenType::TOKEN_NUMBER);
}
void Lexer::handle_hash_comment() {
    if (peek() == ' ') {
        while (peek() != '\n' && !is_at_end()) advance();
        add_token(TokenType::TOKEN_COMMENT_GENERATOR);
    } else {
        add_token(TokenType::TOKEN_HASH);
    }
}
