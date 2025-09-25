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
    size_t startPos = position;
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        value += advance();
    }

    if (value == "inherit") return {TokenType::Inherit, value, line, startCol, startPos};
    if (value == "Origin") return {TokenType::Origin, value, line, startCol, startPos};
    if (value == "use") return {TokenType::Use, value, line, startCol, startPos};
    if (value == "Namespace") return {TokenType::Namespace, value, line, startCol, startPos};
    if (value == "from") return {TokenType::From, value, line, startCol, startPos};
    if (value == "Custom") return {TokenType::Custom, value, line, startCol, startPos};
    if (value == "delete") return {TokenType::Delete, value, line, startCol, startPos};
    if (value == "insert") return {TokenType::Insert, value, line, startCol, startPos};
    if (value == "after") return {TokenType::After, value, line, startCol, startPos};
    if (value == "before") return {TokenType::Before, value, line, startCol, startPos};
    if (value == "replace") return {TokenType::Replace, value, line, startCol, startPos};
    if (value == "at") {
        size_t preWhitespacePos = position;
        int preWhitespaceLine = line;
        int preWhitespaceCol = column;
        skipWhitespace();
        if (peek() == 't' && (position + 3) <= source.length() && source.substr(position, 3) == "top") {
            advance(); advance(); advance();
            return {TokenType::AtTop, "at top", line, startCol, startPos};
        }
        if (peek() == 'b' && (position + 6) <= source.length() && source.substr(position, 6) == "bottom") {
            advance(); advance(); advance(); advance(); advance(); advance();
            return {TokenType::AtBottom, "at bottom", line, startCol, startPos};
        }
        position = preWhitespacePos;
        line = preWhitespaceLine;
        column = preWhitespaceCol;
    }
    if (value == "Import") return {TokenType::Import, value, line, startCol, startPos};
    if (value == "Info") return {TokenType::Info, value, line, startCol, startPos};

    return {TokenType::Identifier, value, line, startCol, startPos};
}

const std::string& Lexer::getSource() const {
    return source;
}

size_t Lexer::getPosition() const {
    return position;
}

void Lexer::setPosition(size_t newPosition) {
    position = newPosition;
}

Token Lexer::stringLiteral(char quoteType) {
    std::string value;
    int startCol = column;
    size_t startPos = position;
    advance(); // consume opening quote
    while (peek() != quoteType && peek() != '\0') {
        value += advance();
    }
    if (peek() == quoteType) {
        advance(); // consume closing quote
    } else {
        return {TokenType::Unexpected, value, line, startCol, startPos};
    }
    return {TokenType::String, value, line, startCol, startPos};
}

Token Lexer::number() {
    std::string value;
    int startCol = column;
    size_t startPos = position;
    while (isdigit(peek())) {
        value += advance();
    }
    if (peek() == '.') {
        value += advance();
        while (isdigit(peek())) {
            value += advance();
        }
    }
    while (isalpha(peek()) || peek() == '%') {
        value += advance();
    }
    return {TokenType::Number, value, line, startCol, startPos};
}

Token Lexer::getNextToken() {
    while (position < source.length()) {
        char current = peek();
        size_t startPos = position;
        int startCol = column;

        if (isspace(current)) {
            skipWhitespace();
            continue;
        }

        if (current == '/' && (position + 1 < source.length())) {
            if (source[position + 1] == '/') {
                advance(); advance();
                skipLineComment();
                continue;
            }
            if (source[position + 1] == '*') {
                advance(); advance();
                skipBlockComment();
                continue;
            }
        }

        if (isdigit(current)) return number();
        if (isalpha(current) || current == '_') return identifier();
        if (current == '"' || current == '\'') return stringLiteral(current);

        if (current == '{') {
            advance();
            if (peek() == '{') {
                advance();
                return {TokenType::OpenDoubleBrace, "{{", line, startCol, startPos};
            }
            return {TokenType::OpenBrace, "{", line, startCol, startPos};
        }

        if (current == '}') {
            advance();
            if (peek() == '}') {
                advance();
                return {TokenType::CloseDoubleBrace, "}}", line, startCol, startPos};
            }
            return {TokenType::CloseBrace, "}", line, startCol, startPos};
        }

        if (current == ':') { advance(); return {TokenType::Colon, ":", line, startCol, startPos}; }
        if (current == ';') { advance(); return {TokenType::Semicolon, ";", line, startCol, startPos}; }
        if (current == '[') { advance(); return {TokenType::OpenBracket, "[", line, startCol, startPos}; }
        if (current == ']') { advance(); return {TokenType::CloseBracket, "]", line, startCol, startPos}; }
        if (current == '@') { advance(); return {TokenType::At, "@", line, startCol, startPos}; }

        if (current == '#') {
            if ((position + 1 < source.length()) && isspace(source[position + 1])) {
                advance(); advance();
                std::string value;
                while (peek() != '\n' && peek() != '\0') {
                    value += advance();
                }
                return {TokenType::HashComment, value, line, startCol, startPos};
            } else {
                advance(); return {TokenType::Hash, "#", line, startCol, startPos};
            }
        }

        if (current == '.') { advance(); return {TokenType::Dot, ".", line, startCol, startPos}; }
        if (current == '$') { advance(); return {TokenType::Dollar, "$", line, startCol, startPos}; }
        if (current == '+') { advance(); return {TokenType::Plus, "+", line, startCol, startPos}; }
        if (current == '-') { advance(); return {TokenType::Minus, "-", line, startCol, startPos}; }
        if (current == '*') { advance(); return {TokenType::Asterisk, "*", line, startCol, startPos}; }
        if (current == '/') { advance(); return {TokenType::Slash, "/", line, startCol, startPos}; }
        if (current == '%') { advance(); return {TokenType::Percent, "%", line, startCol, startPos}; }
        if (current == '(') { advance(); return {TokenType::OpenParen, "(", line, startCol, startPos}; }
        if (current == ')') { advance(); return {TokenType::CloseParen, ")", line, startCol, startPos}; }
        if (current == ',') { advance(); return {TokenType::Comma, ",", line, startCol, startPos}; }
        if (current == '?') { advance(); return {TokenType::QuestionMark, "?", line, startCol, startPos}; }

        if (current == '>') {
            advance();
            if (peek() == '=') { advance(); return {TokenType::GreaterThanEquals, ">=", line, startCol, startPos}; }
            return {TokenType::GreaterThan, ">", line, startCol, startPos};
        }
        if (current == '<') {
            advance();
            if (peek() == '=') { advance(); return {TokenType::LessThanEquals, "<=", line, startCol, startPos}; }
            return {TokenType::LessThan, "<", line, startCol, startPos};
        }
        if (current == '=') {
            advance();
            if (peek() == '=') { advance(); return {TokenType::EqualsEquals, "==", line, startCol, startPos}; }
            return {TokenType::Equals, "=", line, startCol, startPos};
        }
        if (current == '!') {
            advance();
            if (peek() == '=') { advance(); return {TokenType::NotEquals, "!=", line, startCol, startPos}; }
            return {TokenType::Unexpected, "!", line, startCol, startPos};
        }
        if (current == '&') {
            advance();
            if (peek() == '&') { advance(); return {TokenType::LogicalAnd, "&&", line, startCol, startPos}; }
            return {TokenType::Ampersand, "&", line, startCol, startPos};
        }
        if (current == '|') {
            advance();
            if (peek() == '|') { advance(); return {TokenType::LogicalOr, "||", line, startCol, startPos}; }
            return {TokenType::Unexpected, "|", line, startCol, startPos};
        }

        return {TokenType::Unexpected, std::string(1, advance()), line, startCol, startPos};
    }

    return {TokenType::EndOfFile, "", line, column, position};
}