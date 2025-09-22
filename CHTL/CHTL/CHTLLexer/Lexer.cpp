#include "Lexer.h"
#include <string>

namespace CHTL {

Lexer::Lexer(const std::string& input)
    : input(input), position(0), readPosition(0), ch(0), line(1), column(0) {
    readChar();
}

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0; // EOF
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition++;
    column++;
    if (ch == '\n') {
        line++;
        column = 0;
    }
}

char Lexer::peekChar() const {
    if (readPosition >= input.length()) {
        return 0; // EOF
    }
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while (isWhitespace(ch)) {
        readChar();
    }
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Lexer::isLetter(char c) const {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) const {
    return '0' <= c && c <= '9';
}

Token Lexer::readIdentifier() {
    size_t start_pos = position;
    while (isLetter(ch) || isDigit(ch)) {
        readChar();
    }
    return {TokenType::IDENTIFIER, input.substr(start_pos, position - start_pos), line, column};
}

Token Lexer::readStringLiteral(char quote_type) {
    Token token;
    token.line = line;
    token.column = column;
    token.type = TokenType::STRING_LITERAL;

    readChar(); // consume opening quote
    size_t start_pos = position;
    while (ch != quote_type && ch != 0) {
        readChar();
    }

    token.literal = input.substr(start_pos, position - start_pos);

    if (ch == quote_type) {
        readChar(); // consume closing quote
    } else {
        // Unterminated string, maybe report error later
        token.type = TokenType::ILLEGAL;
    }
    return token;
}

Token Lexer::readLineComment() {
    size_t start_pos = position;
    while (ch != '\n' && ch != 0) {
        readChar();
    }
    return {TokenType::LINE_COMMENT, input.substr(start_pos, position - start_pos), line, column};
}

Token Lexer::readBlockComment() {
    size_t start_pos = position;
    readChar(); // consume '*'
    while (!(ch == '*' && peekChar() == '/') && ch != 0) {
        readChar();
    }
    if (ch != 0) {
        readChar(); // consume '*'
        readChar(); // consume '/'
    }
    return {TokenType::BLOCK_COMMENT, input.substr(start_pos, position - start_pos), line, column};
}


Token Lexer::nextToken() {
    Token token;

    skipWhitespace();

    token.line = line;
    token.column = column;

    switch (ch) {
        case '{': token = {TokenType::LBRACE, "{", line, column}; break;
        case '}': token = {TokenType::RBRACE, "}", line, column}; break;
        case '(': token = {TokenType::LPAREN, "(", line, column}; break;
        case ')': token = {TokenType::RPAREN, ")", line, column}; break;
        case '[': token = {TokenType::LBRACKET, "[", line, column}; break;
        case ']': token = {TokenType::RBRACKET, "]", line, column}; break;
        case ':': token = {TokenType::COLON, ":", line, column}; break;
        case ';': token = {TokenType::SEMICOLON, ";", line, column}; break;
        case '=': token = {TokenType::EQUALS, "=", line, column}; break;
        case ',': token = {TokenType::COMMA, ",", line, column}; break;
        case '"': return readStringLiteral('"');
        case '\'': return readStringLiteral('\'');
        case '/':
            if (peekChar() == '/') {
                readChar(); // consume first '/'
                readChar(); // consume second '/'
                return readLineComment();
            } else if (peekChar() == '*') {
                readChar(); // consume '/'
                return readBlockComment();
            }
            // Add other operators starting with '/' later, e.g., division
            token = {TokenType::ILLEGAL, std::string(1, ch), line, column};
            break;
        case 0: token = {TokenType::END_OF_FILE, "", line, column}; break;
        default:
            if (isLetter(ch)) {
                // This will handle identifiers, keywords, and unquoted literals for now
                return readIdentifier();
            }
            // Numbers will be handled later
            token = {TokenType::ILLEGAL, std::string(1, ch), line, column};
            break;
    }

    readChar();
    return token;
}

} // namespace CHTL
