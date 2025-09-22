#include "Lexer.h"
#include <cctype>
#include <map>

namespace CHTL {

// Helper functions to check character types
bool isLetter(char ch) {
    // Allow hyphens for CSS properties like 'background-color'
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-';
}

bool isDigit(char ch) {
    return std::isdigit(static_cast<unsigned char>(ch));
}

// Keyword map
static std::map<std::string, TokenType> keywords = {
    {"delete", TokenType::DELETE},
    {"from", TokenType::FROM}
};

TokenType lookupIdent(const std::string& ident) {
    if (keywords.count(ident)) {
        return keywords.at(ident);
    }
    return TokenType::IDENTIFIER;
}


Lexer::Lexer(std::string input) : m_input(std::move(input)) {
    readChar(); // Initialize the lexer with the first character
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_ch = 0; // EOF
    } else {
        m_ch = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition += 1;
}

char Lexer::peekChar() {
    if (m_readPosition >= m_input.length()) {
        return 0; // EOF
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r') {
        if (m_ch == '\n') {
            m_line++;
        }
        readChar();
    }
}

Token Lexer::nextToken() {
    Token tok;
    skipWhitespace();

    tok.line = m_line;

    switch (m_ch) {
        case '{': tok = {TokenType::LEFT_BRACE, "{", m_line}; break;
        case '}': tok = {TokenType::RIGHT_BRACE, "}", m_line}; break;
        case ':': tok = {TokenType::COLON, ":", m_line}; break;
        case '=': tok = {TokenType::EQUALS, "=", m_line}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", m_line}; break;
        case '.': tok = {TokenType::DOT, ".", m_line}; break;
        case '@': tok = {TokenType::AT_SIGN, "@", m_line}; break;
        case '[': tok = {TokenType::LEFT_BRACKET, "[", m_line}; break;
        case ']': tok = {TokenType::RIGHT_BRACKET, "]", m_line}; break;
        case '(': tok = {TokenType::LEFT_PAREN, "(", m_line}; break;
        case ')': tok = {TokenType::RIGHT_PAREN, ")", m_line}; break;
        case '"':
        case '\'': return readStringLiteral();
        case '/': return readComment();
        case '#':
            if (peekChar() == ' ') {
                return readComment();
            } else {
                tok = {TokenType::HASH, "#", m_line};
            }
            break;
        case 0: tok = {TokenType::END_OF_FILE, "", m_line}; break;
        default:
            if (isLetter(m_ch)) {
                return readIdentifier();
            } else if (isDigit(m_ch)) {
                return readUnquotedLiteral();
            }
            else {
                tok = {TokenType::ILLEGAL, std::string(1, m_ch), m_line};
            }
            break;
    }

    readChar(); // Move to the next character
    return tok;
}

Token Lexer::readIdentifier() {
    size_t start_pos = m_position;
    int start_line = m_line;
    while (isLetter(m_ch) || isDigit(m_ch)) {
        readChar();
    }
    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    TokenType type = lookupIdent(literal);
    return {type, literal, start_line};
}


Token Lexer::readUnquotedLiteral() {
    size_t start_pos = m_position;
    int start_line = m_line;
    // Read a mix of digits, letters, and dots for values like '100px' or '1.5em'
    while (isDigit(m_ch) || isLetter(m_ch) || m_ch == '.') {
        readChar();
    }
    return {TokenType::UNQUOTED_LITERAL, m_input.substr(start_pos, m_position - start_pos), start_line};
}

Token Lexer::readStringLiteral() {
    char quote_type = m_ch;
    size_t start_pos = m_position + 1;
    int start_line = m_line;
    readChar(); // Consume the opening quote

    while (m_ch != quote_type && m_ch != 0) {
        if (m_ch == '\n') m_line++;
        readChar();
    }

    std::string literal = m_input.substr(start_pos, m_position - start_pos);
    readChar(); // Consume the closing quote
    return {TokenType::STRING_LITERAL, literal, start_line};
}

Token Lexer::readComment() {
    size_t start_pos = m_position;
    int start_line = m_line;
    if (m_ch == '#') { // Single-line generator comment
        while (m_ch != '\n' && m_ch != 0) {
            readChar();
        }
    } else if (m_ch == '/' && peekChar() == '/') { // Single-line comment
        while (m_ch != '\n' && m_ch != 0) {
            readChar();
        }
    } else if (m_ch == '/' && peekChar() == '*') { // Multi-line comment
        readChar(); // Consume '/'
        readChar(); // Consume '*'
        while (m_ch != 0 && !(m_ch == '*' && peekChar() == '/')) {
            if (m_ch == '\n') m_line++;
            readChar();
        }
        if (m_ch != 0) readChar(); // Consume '*'
        if (m_ch != 0) readChar(); // Consume '/'
    }

    return {TokenType::COMMENT, m_input.substr(start_pos, m_position - start_pos), start_line};
}

} // namespace CHTL
