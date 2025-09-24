#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source) : _source(source) {}

bool CHTLLexer::isAtEnd() const {
    return _current >= _source.length();
}

char CHTLLexer::peek() const {
    if (isAtEnd()) return '\0';
    return _source[_current];
}

char CHTLLexer::advance() {
    if (!isAtEnd()) {
        if (_source[_current] == '\n') {
            _line++;
            _column = 1;
        } else {
            _column++;
        }
        _current++;
    }
    return _source[_current - 1];
}

Token CHTLLexer::makeToken(TokenType type, const std::string& lexeme) const {
    // A bit of a simplification, but good enough for now.
    return Token{type, lexeme, _line, _column - (int)lexeme.length()};
}

void CHTLLexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance();
                break;
            case '/':
                if (_current + 1 < _source.length() && _source[_current + 1] == '/') {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
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

Token CHTLLexer::makeIdentifier() {
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string lexeme = _source.substr(_start, _current - _start);
    if (lexeme == "text") return makeToken(TokenType::KEYWORD_TEXT, lexeme);
    return makeToken(TokenType::IDENTIFIER, lexeme);
}

Token CHTLLexer::makeString(char quoteType) {
    // Precondition: peek() is the opening quote.
    advance(); // Consume the opening quote.
    _start = _current; // The string's content starts here.

    while (peek() != quoteType && !isAtEnd()) {
        advance();
    }

    if (isAtEnd()) {
        return makeToken(TokenType::UNKNOWN, "Unterminated string.");
    }

    std::string lexeme = _source.substr(_start, _current - _start);
    advance(); // Consume the closing quote.

    return makeToken(TokenType::STRING, lexeme);
}

Token CHTLLexer::getNextToken() {
    skipWhitespaceAndComments();
    _start = _current;

    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE, "EOF");

    char c = peek();

    if (isalpha(c) || c == '_') return makeIdentifier();
    if (isdigit(c)) {
        while (isdigit(peek())) advance();
        return makeToken(TokenType::UNQUOTED_LITERAL, _source.substr(_start, _current - _start));
    }

    // Handle strings separately before the general switch.
    if (c == '"' || c == '\'') {
        return makeString(c);
    }

    // Handle single-character tokens.
    // It's now safe to advance because we've handled all multi-char cases.
    advance();
    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case ':': return makeToken(TokenType::COLON, ":");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case ',': return makeToken(TokenType::COMMA, ",");
    }

    return makeToken(TokenType::UNKNOWN, std::string(1, c));
}

} // namespace CHTL
