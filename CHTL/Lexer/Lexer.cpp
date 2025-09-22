#include "Lexer.h"
#include <iostream>
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scan_tokens() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        start = current;
        Token token = scan_token();
        // We will push all tokens, including UNKNOWN, for the parser to handle.
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) break;
    }
    return tokens;
}

Token Lexer::scan_token() {
    skip_whitespace_and_comments();
    start = current;

    if (is_at_end()) return make_token(TokenType::END_OF_FILE);

    char c = advance();

    switch (c) {
        case '{': return make_token(TokenType::LEFT_BRACE);
        case '}': return make_token(TokenType::RIGHT_BRACE);
        case ':': return make_token(TokenType::COLON);
        case '=': return make_token(TokenType::EQUAL);
        case ';': return make_token(TokenType::SEMICOLON);

        case '.': return make_token(TokenType::DOT);
        case '&': return make_token(TokenType::AMPERSAND);
        case '#': {
            // If '#' is followed by a space, it's a comment.
            // Otherwise, it's an ID selector.
            if (peek() == ' ') {
                // Generator comment: consume the rest of the line.
                while (peek() != '\n' && !is_at_end()) {
                    advance();
                }
                return make_token(TokenType::HASH_COMMENT);
            } else {
                return make_token(TokenType::HASH);
            }
        }

        case '"': return string_literal('"');
        case '\'': return string_literal('\'');
    }

    // Backtrack because the character did not start a simple token
    current--;

    // Now, check for multi-character tokens (identifiers or unquoted literals)
    // This is a context-sensitive problem. A simple lexer might struggle.
    // Let's try a rule: if it starts with a letter, it's an identifier.
    // Everything else is an unquoted literal until a delimiter.

    if (std::isalpha(peek()) || peek() == '_') {
        return identifier();
    }

    // If it's not an identifier, it must be an unquoted literal.
    // This is a fallback. It consumes text until a delimiter.
    return unquoted_literal();
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
                    // A line comment goes until the end of the line.
                    while (peek() != '\n' && !is_at_end()) advance();
                } else if (peek_next() == '*') {
                    // A block comment goes until */
                    advance(); // consume /
                    advance(); // consume *
                    while (!(peek() == '*' && peek_next() == '/') && !is_at_end()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!is_at_end()) {
                        advance(); // consume *
                        advance(); // consume /
                    }
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
        // Only increment column if it's not a newline, which is handled in skip_whitespace
        if (source[current] != '\n') {
            column++;
        }
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

Token Lexer::make_token(TokenType type) const {
    return {type, source.substr(start, current - start), line, (int)(column - (current - start))};
}

Token Lexer::error_token(const std::string& message) const {
     return {TokenType::UNKNOWN, message, line, column};
}

Token Lexer::string_literal(char quote_type) {
    // We already consumed the opening quote in scan_token
    start = current; // The content starts after the quote
    while (peek() != quote_type && !is_at_end()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (is_at_end()) return error_token("Unterminated string.");

    std::string value = source.substr(start, current - start);

    // The closing quote.
    advance();

    return {TokenType::STRING, value, line, (int)(column - value.length() - 1)};
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') advance();

    std::string text = source.substr(start, current - start);
    if (text == "text") {
        return make_token(TokenType::TEXT_KEYWORD);
    }
    // Add other keywords here later, like 'style'
    if (text == "style") {
        // For now, let's make it a generic identifier.
        // The parser can distinguish it.
    }

    return make_token(TokenType::IDENTIFIER);
}

Token Lexer::unquoted_literal() {
    // Greedily consume until a delimiter or end of line.
    // Delimiters are: { } ; : =
    while (!is_at_end()) {
        char c = peek();
        if (c == '\n' || c == '{' || c == '}' || c == ';' || c == ':' || c == '=') {
            break;
        }
        // Also check for comments
        if (c == '/' && (peek_next() == '/' || peek_next() == '*')) {
            break;
        }
        advance();
    }

    // Trim trailing whitespace from the literal
    size_t end = current - 1;
    while (end > start && std::isspace(source[end])) {
        end--;
    }
    std::string value = source.substr(start, end - start + 1);

    // This is tricky, we might have consumed the start of the next token. Let's adjust 'current'.
    current = end + 1;


    if (value.empty()) {
        // This can happen if we only have whitespace.
        // The outer loop in scan_tokens should handle this by re-calling scan_token.
        // Let's return an "UNKNOWN" token that can be skipped.
        return make_token(TokenType::UNKNOWN);
    }

    return {TokenType::UNQUOTED_LITERAL, value, line, (int)(column - value.length())};
}
