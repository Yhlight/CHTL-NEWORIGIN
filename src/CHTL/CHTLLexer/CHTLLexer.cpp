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
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string lexeme = _source.substr(_start, _current - _start);
    if (lexeme == "text") return makeToken(TokenType::KEYWORD_TEXT, lexeme);
    if (lexeme == "style") return makeToken(TokenType::KEYWORD_STYLE, lexeme);
    return makeToken(TokenType::IDENTIFIER, lexeme);
}

Token CHTLLexer::makeString(char quoteType) {
    advance();
    _start = _current;
    while (peek() != quoteType && !isAtEnd()) {
        advance();
    }
    if (isAtEnd()) {
        return makeToken(TokenType::UNKNOWN, "Unterminated string.");
    }
    std::string lexeme = _source.substr(_start, _current - _start);
    advance();
    return makeToken(TokenType::STRING, lexeme);
}

Token CHTLLexer::getNextToken() {
    skipWhitespaceAndComments();
    _start = _current;
    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE, "EOF");
    char c = peek();
    if (isalpha(c) || c == '_') return makeIdentifier();
    if (isdigit(c) || (c == '.' && isdigit(peek()))) {
        while (isdigit(peek()) || peek() == '.') advance();
        while (isalpha(peek()) || peek() == '%') advance();
        return makeToken(TokenType::UNQUOTED_LITERAL, _source.substr(_start, _current - _start));
    }
    if (c == '"' || c == '\'') {
        return makeString(c);
    }
    advance();
    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case ':': return makeToken(TokenType::COLON, ":");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case ',': return makeToken(TokenType::COMMA, ",");
        case '.': return makeToken(TokenType::DOT, ".");
        case '#': return makeToken(TokenType::HASH, "#");
        case '&': return makeToken(TokenType::AMPERSAND, "&");
    }
    return makeToken(TokenType::UNKNOWN, std::string(1, c));
}

} // namespace CHTL
