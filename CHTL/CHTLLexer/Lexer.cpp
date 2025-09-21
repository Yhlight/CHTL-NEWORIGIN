#include "Lexer.h"
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (m_current < m_source.length()) {
        tokens.push_back(nextToken());
    }
    tokens.push_back({TokenType::EndOfFile, "", m_line, m_column});
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();

    if (m_current >= m_source.length()) {
        return {TokenType::EndOfFile, "", m_line, m_column};
    }

    char c = advance();

    if (isalpha(c) || c == '_') {
        std::string value;
        value += c;
        while (isalnum(peek()) || peek() == '_' || peek() == '-') {
            value += advance();
        }
        return {TokenType::Identifier, value, m_line, m_column};
    }

    if (c == '"') {
        std::string value;
        while (peek() != '"' && m_current < m_source.length()) {
            value += advance();
        }
        advance(); // consume the closing quote
        return {TokenType::StringLiteral, value, m_line, m_column};
    }

    switch (c) {
        case '{': return {TokenType::OpenBrace, "{", m_line, m_column};
        case '}': return {TokenType::CloseBrace, "}", m_line, m_column};
        case '[': return {TokenType::OpenBracket, "[", m_line, m_column};
        case ']': return {TokenType::CloseBracket, "]", m_line, m_column};
        case '@': return {TokenType::At, "@", m_line, m_column};
        case ':': return {TokenType::Colon, ":", m_line, m_column};
        case ';': return {TokenType::Semicolon, ";", m_line, m_column};
    }

    return {TokenType::Unknown, std::string(1, c), m_line, m_column};
}

void Lexer::skipWhitespaceAndComments() {
    while (m_current < m_source.length()) {
        char c = peek();
        if (isspace(c)) {
            if (c == '\n') {
                m_line++;
                m_column = 1;
            } else {
                m_column++;
            }
            advance();
        } else if (c == '/' && m_current + 1 < m_source.length() && m_source[m_current + 1] == '/') {
            // Single line comment
            while (peek() != '\n' && m_current < m_source.length()) {
                advance();
            }
        } else {
            break;
        }
    }
}

char Lexer::advance() {
    m_column++;
    return m_source[m_current++];
}

char Lexer::peek() {
    if (m_current >= m_source.length()) return '\0';
    return m_source[m_current];
}

} // namespace CHTL
