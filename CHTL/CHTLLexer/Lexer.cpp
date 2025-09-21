#include "Lexer.h"
#include <cctype> // for isalpha, isdigit

namespace CHTL {

Lexer::Lexer(const std::string& source) : source(source) {}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
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

Token Lexer::makeToken(TokenType type) const {
    return Token{type, source.substr(start, current - start), line, column - (int)(current - start)};
}

Token Lexer::errorToken(const std::string& message) const {
    return Token{TokenType::TOKEN_ERROR, message, line, column};
}

void Lexer::skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 0; // It will be 1 after the advance()
                advance();
                break;
            default:
                return;
        }
    }
}

bool Lexer::isIdentifierChar(char c) {
    return isalpha(c) || isdigit(c) || c == '_';
}

Token Lexer::makeIdentifier() {
    while (isIdentifierChar(peek())) {
        advance();
    }
    // CHTL allows hyphens in identifiers (e.g., for CSS classes)
    // This simple check might need to be expanded based on full language spec.
    while (peek() == '-' && isIdentifierChar(peekNext())) {
        advance(); // consume '-'
        while (isIdentifierChar(peek())) {
            advance();
        }
    }
    return makeToken(TokenType::IDENTIFIER);
}

Token Lexer::makeString(char quoteType) {
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    // Consume the closing quote.
    advance();

    // The value is the substring without the quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    // Create a token with the raw lexeme for now. The parser can unwrap.
    return makeToken(TokenType::STRING);
}

Token Lexer::makeComment() {
    // This handles //, /* */, and # comments.
    if (peek() == '/') { // Single line comment
        while (peek() != '\n' && !isAtEnd()) {
            advance();
        }
    } else if (peek() == '*') { // Multi-line comment
        advance(); // consume '*'
        while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
            if (peek() == '\n') {
                line++;
                column = 0;
            }
            advance();
        }
        if (isAtEnd()) return errorToken("Unterminated multi-line comment.");
        advance(); // consume '*'
        advance(); // consume '/'
    }
    return makeToken(TokenType::COMMENT);
}

Token Lexer::scanToken() {
    skipWhitespace();
    start = current;
    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance();

    if (isalpha(c) || c == '_') return makeIdentifier();

    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case ':': return makeToken(TokenType::COLON);
        case ';': return makeToken(TokenType::SEMICOLON);
        case '=': return makeToken(TokenType::EQUAL);
        case '"': return makeString('"');
        case '\'': return makeString('\'');
        case '#':
            // CHTL spec: "#" must be followed by a space to be a generator comment.
            if (peek() == ' ') {
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                return makeToken(TokenType::COMMENT);
            }
            // If not followed by a space, it could be an ID selector in CSS.
            // For now, we'll treat it as part of an identifier.
            // This logic is tricky. A better way might be to have the lexer
            // behave differently based on context (e.g., inside a style block).
            // For this first version, we'll tokenize it as an identifier if it's not a comment.
            while (isIdentifierChar(peek()) || peek() == '-') advance();
            return makeToken(TokenType::IDENTIFIER);

        case '/':
            if (match('/') || match('*')) {
                // It's a comment. Backtrack one char for makeComment to see the type.
                current--;
                column--;
                return makeComment();
            }
            // Later, this could be a division operator. For now, it's an error.
            return errorToken("Unexpected character: /");
        default:
            break;
    }

    return errorToken("Unexpected character.");
}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        tokens.push_back(scanToken());
        // The EOF token is added outside the loop
        if (tokens.back().type == TokenType::TOKEN_EOF) break;
    }
    // Ensure EOF token is the last one, even if source is empty.
    if (tokens.empty() || tokens.back().type != TokenType::TOKEN_EOF) {
        start = current;
        tokens.push_back(makeToken(TokenType::TOKEN_EOF));
    }
    return tokens;
}

} // namespace CHTL
