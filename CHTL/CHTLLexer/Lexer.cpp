#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(std::string input)
    : m_input(std::move(input)), m_position(0), m_readPosition(0), m_char(0) {
    readChar();
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition += 1;
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (isspace(m_char)) {
        readChar();
    }
}

bool isIdentifierStart(char ch) {
    return isalpha(ch) || ch == '_';
}

bool isIdentifierChar(char ch) {
    return isalnum(ch) || ch == '_';
}

Token Lexer::readIdentifier() {
    size_t startPosition = m_position;
    while (isIdentifierStart(m_char)) {
        readChar();
    }
    return {TokenType::Identifier, m_input.substr(startPosition, m_position - startPosition)};
}

Token Lexer::readString(char quote) {
    size_t startPosition = m_position + 1;
    do {
        readChar();
    } while (m_char != quote && m_char != 0);

    std::string literal = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // Consume the closing quote
    return {TokenType::String, literal};
}

// For now, an unquoted string is treated similarly to an identifier.
// The parser will provide context. This handles values like `red` or `box`.
Token Lexer::readUnquotedString() {
    size_t startPosition = m_position;
    while (m_char != 0 && !isspace(m_char) && m_char != ';' && m_char != '}' && m_char != '{' && m_char != ':') {
        readChar();
    }
    return {TokenType::String, m_input.substr(startPosition, m_position - startPosition)};
}


Token Lexer::NextToken() {
    skipWhitespace();

    Token tok(TokenType::Illegal, std::string(1, m_char));

    switch (m_char) {
        case '{':
            tok = Token(TokenType::LBrace, "{");
            break;
        case '}':
            tok = Token(TokenType::RBrace, "}");
            break;
        case ':':
            tok = Token(TokenType::Colon, ":");
            break;
        case ';':
            tok = Token(TokenType::Semicolon, ";");
            break;
        case '=':
            tok = Token(TokenType::Equals, "=");
            break;
        case '"':
        case '\'':
            return readString(m_char);
        case 0:
            tok = Token(TokenType::Eof, "");
            break;
        default:
            if (isIdentifierStart(m_char)) {
                // It could be an identifier (tag, property name) or an unquoted string literal.
                // We'll tokenize as Identifier and let the parser decide.
                // For example, in `color: red;`, both `color` and `red` would be identifiers.
                size_t start = m_position;
                while(isalnum(m_char) || m_char == '_') {
                    readChar();
                }
                std::string literal = m_input.substr(start, m_position - start);
                return {TokenType::Identifier, literal};
            }
            // For now, we don't support unquoted strings that start with non-alpha chars
            // to avoid ambiguity. This will be expanded later.
            tok = Token(TokenType::Illegal, std::string(1, m_char));
            readChar();
            return tok;
    }

    readChar();
    return tok;
}

} // CHTL
