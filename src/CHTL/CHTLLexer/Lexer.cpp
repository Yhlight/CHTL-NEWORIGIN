#include "Lexer.h"
#include <cctype>

namespace CHTL {

namespace { // Anonymous namespace for helper functions
    bool isLetter(char ch) {
        return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
    }
}

Lexer::Lexer(const std::string& input)
    : input(input), position(0), readPosition(0), ch(0), line(1), column(0) {
    readChar(); // Initialize the first character
}

void Lexer::readChar() {
    if (ch == '\n' || ch == '\r') {
        if (ch == '\r' && peekChar() == '\n') {
             // Handle CRLF
            ch = input[readPosition];
            position = readPosition;
            readPosition += 1;
        }
        line++;
        column = 0;
    }

    if (readPosition >= input.length()) {
        ch = 0; // EOF
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition += 1;
    column++;
}

char Lexer::peekChar() const {
    if (readPosition >= input.length()) {
        return 0; // EOF
    }
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        readChar();
    }
}

Token Lexer::readIdentifier() {
    size_t start_pos = position;
    int start_col = column;
    while (isLetter(ch) || std::isdigit(ch)) { // Identifiers can contain numbers
        readChar();
    }
    std::string literal = input.substr(start_pos, position - start_pos);
    return {IDENTIFIER, literal, line, start_col};
}

Token Lexer::readString() {
    size_t start_pos = position + 1; // Skip the opening "
    int start_col = column;
    readChar(); // Consume opening "
    while (ch != '"' && ch != 0) {
        readChar();
    }

    if (ch == 0) { // Unterminated string
        return {ILLEGAL, input.substr(start_pos - 1, position - (start_pos - 1)), line, start_col};
    }

    std::string literal = input.substr(start_pos, position - start_pos);
    readChar(); // Consume closing "
    return {STRING, literal, line, start_col};
}


Token Lexer::nextToken() {
    skipWhitespace();

    Token tok;
    tok.line = this->line;
    tok.column = this->column;

    switch (ch) {
        case '{':
            tok.type = LBRACE;
            tok.literal = "{";
            break;
        case '}':
            tok.type = RBRACE;
            tok.literal = "}";
            break;
        case ':':
            tok.type = COLON;
            tok.literal = ":";
            break;
        case ';':
            tok.type = SEMICOLON;
            tok.literal = ";";
            break;
        case '"':
            return readString();
        case 0:
            tok.type = END_OF_FILE;
            tok.literal = "";
            break;
        default:
            if (isLetter(ch)) {
                return readIdentifier();
            } else {
                tok.type = ILLEGAL;
                tok.literal = std::string(1, ch);
            }
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL
