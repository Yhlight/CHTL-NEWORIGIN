#include "Lexer.h"
#include <iostream>
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::TOKEN_EOF, "", line, 0});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::TOKEN_LBRACE); break;
        case '}': addToken(TokenType::TOKEN_RBRACE); break;
        case '(': addToken(TokenType::TOKEN_LPAREN); break;
        case ')': addToken(TokenType::TOKEN_RPAREN); break;
        case '[': addToken(TokenType::TOKEN_LBRACKET); break;
        case ']': addToken(TokenType::TOKEN_RBRACKET); break;
        case ':': addToken(TokenType::TOKEN_COLON); break;
        case ';': addToken(TokenType::TOKEN_SEMICOLON); break;
        case ',': addToken(TokenType::TOKEN_COMMA); break;
        case '.': addToken(TokenType::TOKEN_DOT); break;
        case '-': addToken(TokenType::TOKEN_MINUS); break;
        case '+': addToken(TokenType::TOKEN_PLUS); break;
        case '%': addToken(TokenType::TOKEN_PERCENT); break;
        case '?': addToken(TokenType::TOKEN_QUESTION); break;
        case '&': addToken(TokenType::TOKEN_AMPERSAND); break;

        case '*':
            addToken(match('*') ? TokenType::TOKEN_DOUBLE_STAR : TokenType::TOKEN_STAR);
            break;
        case '=':
            addToken(TokenType::TOKEN_EQUALS);
            break;

        case '/':
            if (match('/')) {
                // A single-line comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
                addToken(TokenType::TOKEN_COMMENT, source.substr(start, current - start));
            } else if (match('*')) {
                blockComment();
                // The block comment itself doesn't add the token, the scanner does.
                addToken(TokenType::TOKEN_COMMENT, source.substr(start, current - start));
            }
            else {
                addToken(TokenType::TOKEN_SLASH);
            }
            break;

        case '#':
            // Generator comment.
            while (peek() != '\n' && !isAtEnd()) advance();
            addToken(TokenType::TOKEN_GENERATOR_COMMENT, source.substr(start, current - start));
            break;

        // Ignore whitespace.
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;

        case '"':
        case '\'':
            string(c);
            break;

        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            } else {
                // For CHTL's unquoted literals, we might need a more sophisticated check,
                // but for now, we can treat them as a form of identifier or a special literal.
                // The parser will ultimately decide the context.
                // Let's assume unquoted literals can start with non-standard characters too.
                // This part will need refinement.
                identifier(); // Treat as an identifier/unquoted literal for now
            }
            break;
    }
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, start});
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::string(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string.
        tokens.push_back({TokenType::TOKEN_ERROR, "Unterminated string.", line, start});
        return;
    }

    // The closing quote.
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::TOKEN_STRING, value);
}

void Lexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the ".".
        advance();
        while (isdigit(peek())) advance();
    }

    // Also handle units like 'px', '%', etc. which can follow a number.
    // For now, the lexer will include them as part of the number token's lexeme.
    while (isalpha(peek()) || peek() == '%') advance();


    addToken(TokenType::TOKEN_NUMBER, source.substr(start, current - start));
}

void Lexer::identifier() {
    // CHTL allows a wide range of characters in unquoted literals.
    // A simple rule could be to consume until a character that acts as a delimiter.
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();

    // The parser will distinguish keywords from identifiers.
    addToken(TokenType::TOKEN_IDENTIFIER);
}

void Lexer::blockComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        tokens.push_back({TokenType::TOKEN_ERROR, "Unterminated block comment.", line, start});
        return;
    }

    // Consume */
    advance();
    advance();
}
