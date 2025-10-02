#include "CHTLLexer.h"
#include <cctype>

CHTLLexer::CHTLLexer(const std::string& input)
    : input(input), position(0), line(1), column(1) {}

char CHTLLexer::currentChar() {
    if (position >= input.length()) {
        return '\0';
    }
    return input[position];
}

void CHTLLexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    position++;
}

void CHTLLexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

void CHTLLexer::skipSingleLineComment() {
    while (currentChar() != '\0' && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipMultiLineComment() {
    while (currentChar() != '\0') {
        if (currentChar() == '*' && (position + 1 < input.length() && input[position + 1] == '/')) {
            advance();
            advance();
            break;
        }
        advance();
    }
}

Token CHTLLexer::makeIdentifier() {
    std::string value;
    size_t start_pos = position;
    // Allow hyphens and dots for CSS properties and values.
    while (currentChar() != '\0' && (isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-' || currentChar() == '.')) {
        value += currentChar();
        advance();
    }

    if (value == "from") {
        return makeToken(TokenType::From, value);
    }
    if (value == "as") {
        return makeToken(TokenType::As, value);
    }

    return makeToken(TokenType::Identifier, value);
}

Token CHTLLexer::makeString(char quote) {
    std::string value;
    advance(); // Consume opening quote
    while (currentChar() != '\0' && currentChar() != quote) {
        value += currentChar();
        advance();
    }
    advance(); // Consume closing quote
    return makeToken(TokenType::String, value);
}

Token CHTLLexer::makeComment() {
    std::string value;
    advance(); // Consume '#'
    if (currentChar() == ' ') {
        advance(); // Consume space
        while (currentChar() != '\0' && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
    }
    return makeToken(TokenType::Comment, value);
}

Token CHTLLexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, line, column};
}

Token CHTLLexer::getNextToken() {
    while (currentChar() != '\0') {
        if (isspace(currentChar())) {
            skipWhitespace();
            continue;
        }

        if (currentChar() == '/' && (position + 1 < input.length())) {
            if (input[position + 1] == '/') {
                advance();
                advance();
                skipSingleLineComment();
                continue;
            } else if (input[position + 1] == '*') {
                advance();
                advance();
                skipMultiLineComment();
                continue;
            }
        }

        if (currentChar() == '#') {
            return makeComment();
        }

        if (currentChar() == '\'' || currentChar() == '"') {
            return makeString(currentChar());
        }

        // Allow identifiers to start with a digit for CSS values like '16px'.
        if (isalpha(currentChar()) || currentChar() == '_' || isdigit(currentChar())) {
            return makeIdentifier();
        }

        if (currentChar() == '{') {
            advance();
            return makeToken(TokenType::LBrace, "{");
        }

        if (currentChar() == '}') {
            advance();
            return makeToken(TokenType::RBrace, "}");
        }

        if (currentChar() == '[') {
            advance();
            return makeToken(TokenType::LBracket, "[");
        }

        if (currentChar() == ']') {
            advance();
            return makeToken(TokenType::RBracket, "]");
        }

        if (currentChar() == '@') {
            advance();
            return makeToken(TokenType::At, "@");
        }

        if (currentChar() == ':') {
            advance();
            return makeToken(TokenType::Colon, ":");
        }

        if (currentChar() == ';') {
            advance();
            return makeToken(TokenType::Semicolon, ";");
        }

        if (currentChar() == '=') {
            advance();
            return makeToken(TokenType::Assign, "=");
        }

        if (currentChar() == ',') {
            advance();
            return makeToken(TokenType::Comma, ",");
        }

        // Unrecognized character
        advance();
        return makeToken(TokenType::Unknown, std::string(1, input[position - 1]));
    }

    return makeToken(TokenType::EndOfFile, "");
}

std::vector<Token> CHTLLexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    while (token.type != TokenType::EndOfFile) {
        tokens.push_back(token);
        token = getNextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}