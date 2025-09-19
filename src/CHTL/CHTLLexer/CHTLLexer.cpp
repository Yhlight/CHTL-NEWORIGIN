#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <cstring>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source)
    : source(source), current_pos(0), line(1), column(1) {}

void CHTLLexer::skipWhitespace() {
    while (peek() != '\0' && isspace(peek())) {
        advance();
    }
}

void CHTLLexer::skipComment() {
    if (peek() == '#') {
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    }
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = getNextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}

Token CHTLLexer::getNextToken() {
    while (peek() != '\0') {
        if (isspace(peek())) {
            skipWhitespace();
            continue;
        }

        if (peek() == '#') {
            skipComment();
            continue;
        }

        if (isdigit(peek())) {
            return makeNumber();
        }

        if (isalpha(peek()) || peek() == '_') {
            return makeIdentifier();
        }

        if (peek() == '"') {
            return makeString();
        }

        switch (peek()) {
            case '[': {
                int start_col = column;
                advance();
                return {TokenType::LEFT_BRACKET, "[", line, start_col};
            }
            case ']': {
                int start_col = column;
                advance();
                return {TokenType::RIGHT_BRACKET, "]", line, start_col};
            }
            case '@': {
                int start_col = column;
                advance();
                return {TokenType::AT_SIGN, "@", line, start_col};
            }
            case '+': {
                int start_col = column;
                advance();
                return {TokenType::PLUS, "+", line, start_col};
            }
            case '-': {
                int start_col = column;
                advance();
                return {TokenType::MINUS, "-", line, start_col};
            }
            case '*': {
                int start_col = column;
                advance();
                if (peek() == '*') {
                    advance();
                    return {TokenType::POWER, "**", line, start_col};
                }
                return {TokenType::STAR, "*", line, start_col};
            }
            case '/': {
                int start_col = column;
                advance();
                return {TokenType::SLASH, "/", line, start_col};
            }
            case '%': {
                int start_col = column;
                advance();
                return {TokenType::PERCENT, "%", line, start_col};
            }
            case '{': {
                int start_col = column;
                advance();
                return {TokenType::LEFT_BRACE, "{", line, start_col};
            }
            case '}': {
                int start_col = column;
                advance();
                return {TokenType::RIGHT_BRACE, "}", line, start_col};
            }
            case ':': {
                int start_col = column;
                advance();
                return {TokenType::COLON, ":", line, start_col};
            }
            case '=': {
                int start_col = column;
                advance();
                return {TokenType::EQUALS, "=", line, start_col};
            }
            case ';': {
                int start_col = column;
                advance();
                return {TokenType::SEMICOLON, ";", line, start_col};
            }
            case '.': {
                int start_col = column;
                advance();
                return {TokenType::DOT, ".", line, start_col};
            }
            case '#': {
                int start_col = column;
                advance();
                return {TokenType::HASH, "#", line, start_col};
            }
            case '&': {
                int start_col = column;
                advance();
                return {TokenType::AMPERSAND, "&", line, start_col};
            }
        }

        // If nothing matches, maybe it's an unquoted literal or an unknown token.
        // For now, let's treat other characters that are not special as part of unquoted literals.
        if (!isspace(peek()) && strchr("{}:;=\"", peek()) == NULL) {
            return makeUnquotedLiteral();
        }

        char c = advance();
        return {TokenType::UNKNOWN, std::string(1, c), line, column-1};
    }

    return {TokenType::END_OF_FILE, "", line, column};
}

char CHTLLexer::peek() {
    if (current_pos >= source.length()) return '\0';
    return source[current_pos];
}

char CHTLLexer::advance() {
    char ch = peek();
    if (ch != '\0') {
        current_pos++;
        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    return ch;
}

Token CHTLLexer::makeIdentifier() {
    std::string value;
    int start_col = column;
    // Allow letters, numbers, underscore, and hyphen in identifiers
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        value += advance();
    }

    if (value == "text") return {TokenType::KEYWORD_TEXT, value, line, start_col};
    if (value == "style") return {TokenType::KEYWORD_STYLE, value, line, start_col};
    if (value == "script") return {TokenType::KEYWORD_SCRIPT, value, line, start_col};

    return {TokenType::IDENTIFIER, value, line, start_col};
}

Token CHTLLexer::makeString() {
    std::string value;
    int start_col = column;
    advance(); // consume opening quote
    while (peek() != '"' && peek() != '\0') {
        value += advance();
    }
    if (peek() == '"') {
        advance(); // consume closing quote
    }
    return {TokenType::STRING_LITERAL, value, line, start_col};
}

Token CHTLLexer::makeUnquotedLiteral() {
    std::string value;
    int start_col = column;
    while (peek() != '\0' && !isspace(peek()) && strchr("{}:;=\"#", peek()) == NULL) {
        value += advance();
    }
    return {TokenType::UNQUOTED_LITERAL, value, line, start_col};
}

Token CHTLLexer::makeNumber() {
    std::string value;
    int start_col = column;
    bool has_decimal = false;
    while (isdigit(peek()) || (peek() == '.' && !has_decimal)) {
        if (peek() == '.') {
            has_decimal = true;
        }
        value += advance();
    }
    return {TokenType::NUMBER, value, line, start_col};
}

} // namespace CHTL
