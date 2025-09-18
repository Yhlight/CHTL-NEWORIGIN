#include "CHTL/CHTLLexer/Lexer.h"
#include <cctype>
#include <unordered_map>
#include <string>

// Helper map to distinguish keywords from identifiers
std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::TEXT}
};

Lexer::Lexer(const std::string& input)
    : m_input(input), m_position(0), m_readPosition(0), m_char(0), m_line(1) {
    readChar(); // Initialize the first character
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // Use 0 to represent EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition += 1;
}

char Lexer::peekChar() {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (true) {
        if (isspace(m_char)) {
            if (m_char == '\n') {
                m_line++;
            }
            readChar();
        } else if (m_char == '/') {
            if (peekChar() == '/') { // Single-line comment
                readChar(); readChar();
                while (m_char != '\n' && m_char != 0) {
                    readChar();
                }
            } else if (peekChar() == '*') { // Multi-line comment
                readChar(); readChar();
                while (m_char != 0) {
                    if (m_char == '\n') m_line++;
                    if (m_char == '*' && peekChar() == '/') {
                        readChar(); readChar();
                        break;
                    }
                    readChar();
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

// This function now reads both standard identifiers and unquoted literals.
Token Lexer::readIdentifier() {
    size_t startPosition = m_position;
    // An identifier or unquoted literal is a sequence of characters
    // that are not special delimiters.
    while (m_char != 0 && !isspace(m_char) &&
           m_char != '{' && m_char != '}' && m_char != ':' &&
           m_char != '=' && m_char != ';' && m_char != '"' && m_char != '#') {
        readChar();
    }
    std::string literal = m_input.substr(startPosition, m_position - startPosition);

    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        return {it->second, literal, m_line};
    }

    return {TokenType::IDENTIFIER, literal, m_line};
}

Token Lexer::readString() {
    readChar(); // Consume the opening "
    size_t startPosition = m_position;

    while (m_char != '"' && m_char != 0) {
        if (m_char == '\n') m_line++;
        readChar();
    }

    if (m_char == 0) {
        return {TokenType::ILLEGAL, "Unterminated String", m_line};
    }

    std::string literal = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // Consume the closing "
    return {TokenType::STRING, literal, m_line};
}

Token Lexer::readGeneratorComment() {
    readChar(); // consume '#'
    if (m_char != ' ') {
        return {TokenType::ILLEGAL, "#", m_line};
    }
    readChar(); // consume ' '

    size_t startPosition = m_position;
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
    std::string literal = m_input.substr(startPosition, m_position - startPosition);
    return {TokenType::GENERATOR_COMMENT, literal, m_line};
}

Token Lexer::nextToken() {
    Token tok;

    skipWhitespace();

    switch (m_char) {
        case '{': tok = {TokenType::LEFT_BRACE, "{", m_line}; break;
        case '}': tok = {TokenType::RIGHT_BRACE, "}", m_line}; break;
        case ':': tok = {TokenType::COLON, ":", m_line}; break;
        case '=': tok = {TokenType::EQUALS, "=", m_line}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", m_line}; break;
        case '"': return readString();
        case '#': return readGeneratorComment();
        case 0: tok = {TokenType::END_OF_FILE, "", m_line}; break;
        default:
            // If it's not a special character, it must be the start of
            // an identifier or an unquoted literal.
            return readIdentifier();
    }

    readChar();
    return tok;
}
