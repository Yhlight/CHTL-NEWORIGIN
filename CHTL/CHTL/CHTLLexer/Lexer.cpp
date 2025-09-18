#include "CHTL/CHTLLexer/Lexer.h"
#include <cctype>
#include <unordered_map>

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
    while (isspace(m_char)) {
        if (m_char == '\n') {
            m_line++;
        }
        readChar();
    }
}

Token Lexer::readIdentifier() {
    size_t startPosition = m_position;
    // Identifiers can contain letters, numbers, and underscores, but can't start with a number.
    // For CHTL element names, we'll stick to alphabetic characters for now.
    while (isalpha(m_char)) {
        readChar();
    }
    std::string literal = m_input.substr(startPosition, m_position - startPosition);

    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        // It's a keyword
        return {it->second, literal, m_line};
    }

    // It's a user-defined identifier
    return {TokenType::IDENTIFIER, literal, m_line};
}

Token Lexer::readString() {
    readChar(); // Consume the opening "
    size_t startPosition = m_position;

    while (m_char != '"' && m_char != 0) {
        if (m_char == '\n') m_line++; // Strings can be multi-line
        readChar();
    }

    if (m_char == 0) {
        // Unterminated string
        return {TokenType::ILLEGAL, "Unterminated String", m_line};
    }

    std::string literal = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // Consume the closing "
    return {TokenType::STRING, literal, m_line};
}

Token Lexer::nextToken() {
    Token tok;

    skipWhitespace();

    switch (m_char) {
        case '{':
            tok = {TokenType::LEFT_BRACE, "{", m_line};
            break;
        case '}':
            tok = {TokenType::RIGHT_BRACE, "}", m_line};
            break;
        case '"':
            // This case handles the start of a string literal
            return readString();
        case 0:
            tok = {TokenType::END_OF_FILE, "", m_line};
            break;
        default:
            if (isalpha(m_char)) {
                // If it starts with a letter, it's either an identifier or a keyword
                return readIdentifier();
            } else {
                // Unrecognized character
                tok = {TokenType::ILLEGAL, std::string(1, m_char), m_line};
            }
            break;
    }

    readChar(); // Move to the next character
    return tok;
}
