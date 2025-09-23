#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char Lexer::peek() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::advance() {
    if (position >= source.length()) {
        return '\0';
    }
    char current = source[position];
    position++;
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current;
}

void Lexer::skipWhitespace() {
    while (position < source.length() && isspace(peek())) {
        advance();
    }
}

void Lexer::skipLineComment() {
    while (peek() != '\n' && peek() != '\0') {
        advance();
    }
}

void Lexer::skipBlockComment() {
    // Assumes '/*' has already been consumed.
    while (position < source.length()) {
        if (peek() == '*' && (position + 1 < source.length()) && source[position + 1] == '/') {
            advance(); // consume '*'
            advance(); // consume '/'
            return;
        }
        advance();
    }
    // Unterminated block comment, might be an error condition to handle later.
}

Token Lexer::identifier() {
    std::string value;
    int startCol = column;
    while (isalnum(peek()) || peek() == '_') {
        value += advance();
    }
    return {TokenType::Identifier, value, line, startCol};
}

Token Lexer::stringLiteral(char quoteType) {
    std::string value;
    int startCol = column;
    advance(); // consume opening quote
    while (peek() != quoteType && peek() != '\0') {
        // CHTL spec doesn't mention escape characters, so we treat them literally for now.
        value += advance();
    }
    if (peek() == quoteType) {
        advance(); // consume closing quote
    } else {
        // Unterminated string literal.
        return {TokenType::Unexpected, value, line, startCol};
    }
    return {TokenType::String, value, line, startCol};
}


Token Lexer::number() {
    std::string value;
    int startCol = column;

    // Integer part
    while (isdigit(peek())) {
        value += advance();
    }

    // Decimal part
    if (peek() == '.') {
        value += advance();
        while (isdigit(peek())) {
            value += advance();
        }
    }

    // Unit part (e.g., px, em, %)
    while (isalpha(peek()) || peek() == '%') {
        value += advance();
    }

    return {TokenType::Number, value, line, startCol};
}

Token Lexer::getNextToken() {
    while (position < source.length()) {
        char current = peek();

        if (isspace(current)) {
            skipWhitespace();
            continue;
        }

        // Comments to be ignored by the parser
        if (current == '/' && (position + 1 < source.length())) {
            if (source[position + 1] == '/') {
                advance(); advance(); // Consume '//'
                skipLineComment();
                continue;
            }
            if (source[position + 1] == '*') {
                advance(); advance(); // Consume '/*'
                skipBlockComment();
                continue;
            }
        }

        if (isdigit(current)) {
            return number();
        }

        if (isalpha(current) || current == '_') {
            return identifier();
        }

        if (current == '"' || current == '\'') {
            return stringLiteral(current);
        }

        if (current == '{') {
            int startCol = column; advance();
            return {TokenType::OpenBrace, "{", line, startCol};
        }

        if (current == '}') {
            int startCol = column; advance();
            return {TokenType::CloseBrace, "}", line, startCol};
        }

        if (current == ':') {
            int startCol = column; advance();
            return {TokenType::Colon, ":", line, startCol};
        }

        if (current == '=') {
            int startCol = column; advance();
            return {TokenType::Equals, "=", line, startCol};
        }

        if (current == ';') {
            int startCol = column; advance();
            return {TokenType::Semicolon, ";", line, startCol};
        }

        if (current == '#') {
            // If # is followed by a space, it's a comment. Otherwise, it's a selector Hash.
            if ((position + 1 < source.length()) && isspace(source[position + 1])) {
                int startCol = column;
                advance(); // consume '#'
                advance(); // consume ' '
                std::string value;
                while (peek() != '\n' && peek() != '\0') {
                    value += advance();
                }
                return {TokenType::HashComment, value, line, startCol};
            } else {
                int startCol = column; advance(); return {TokenType::Hash, "#", line, startCol};
            }
        }

        // CSS Selector tokens
        if (current == '.') { int startCol = column; advance(); return {TokenType::Dot, ".", line, startCol}; }
        if (current == '&') { int startCol = column; advance(); return {TokenType::Ampersand, "&", line, startCol}; }

        // Arithmetic and grouping operators
        if (current == '+') { int startCol = column; advance(); return {TokenType::Plus, "+", line, startCol}; }
        if (current == '-') { int startCol = column; advance(); return {TokenType::Minus, "-", line, startCol}; }
        if (current == '*') { int startCol = column; advance(); return {TokenType::Asterisk, "*", line, startCol}; }
        if (current == '/') { int startCol = column; advance(); return {TokenType::Slash, "/", line, startCol}; }
        if (current == '%') { int startCol = column; advance(); return {TokenType::Percent, "%", line, startCol}; }
        if (current == '(') { int startCol = column; advance(); return {TokenType::OpenParen, "(", line, startCol}; }
        if (current == ')') { int startCol = column; advance(); return {TokenType::CloseParen, ")", line, startCol}; }

        // If we reach here, the character is not part of any recognized token.
        int startCol = column;
        return {TokenType::Unexpected, std::string(1, advance()), line, startCol};
    }

    return {TokenType::EndOfFile, "", line, column};
}
