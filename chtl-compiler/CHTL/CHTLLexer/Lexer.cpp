#include "Lexer.h"
#include <cctype>
#include <map>

static std::map<std::string, TokenType> keywords = {
    {"delete", TokenType::DeleteKeyword},
    {"from", TokenType::FromKeyword},
    {"as", TokenType::AsKeyword},
    {"style", TokenType::Identifier}, // Treat style as a regular identifier for now
    {"text", TokenType::Identifier}, // Treat text as a regular identifier
    {"Template", TokenType::Identifier}, // Treat Template as a regular identifier
    {"Custom", TokenType::Identifier},
    {"Import", TokenType::Identifier}
};

Lexer::Lexer(const std::string& source) : source(source) {}

char Lexer::currentChar() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

void Lexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    position++;
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

Token Lexer::identifier() {
    std::string value;
    int start_line = line;
    int start_column = column;
    while (currentChar() != '\0' && (isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-' || currentChar() == '.' || currentChar() == '%')) {
        value += currentChar();
        advance();
    }

    if (keywords.count(value)) {
        return {keywords.at(value), value, start_line, start_column};
    }

    return {TokenType::Identifier, value, start_line, start_column};
}

Token Lexer::stringLiteral() {
    std::string value;
    int start_line = line;
    int start_column = column;
    advance(); // Consume the opening quote
    while (currentChar() != '\0' && currentChar() != '"') {
        value += currentChar();
        advance();
    }
    advance(); // Consume the closing quote
    return {TokenType::StringLiteral, value, start_line, start_column};
}

Token Lexer::getNextToken() {
    skipWhitespace();

    char current = currentChar();

    if (current == '\0') {
        return {TokenType::EndOfFile, "", line, column};
    }

    if (isalnum(current) || current == '_' || current == '-' || current == '.') {
        return identifier();
    }

    if (current == '"') {
        return stringLiteral();
    }

    if (current == '{') {
        advance();
        return {TokenType::LeftBrace, "{", line, column - 1};
    }

    if (current == '}') {
        advance();
        return {TokenType::RightBrace, "}", line, column - 1};
    }

    if (current == ':') {
        advance();
        return {TokenType::Colon, ":", line, column - 1};
    }

    if (current == '=') {
        advance();
        return {TokenType::Equal, "=", line, column - 1};
    }

    if (current == ';') {
        advance();
        return {TokenType::Semicolon, ";", line, column - 1};
    }


    if (current == '#') {
        return generatorComment();
    }

    if (current == '[') {
        advance();
        return {TokenType::LeftBracket, "[", line, column - 1};
    }

    if (current == ']') {
        advance();
        return {TokenType::RightBracket, "]", line, column - 1};
    }

    if (current == '@') {
        advance();
        return {TokenType::At, "@", line, column - 1};
    }

    if (current == '+') {
        advance();
        return {TokenType::Plus, "+", line, column - 1};
    }

    if (current == '-') {
        advance();
        return {TokenType::Minus, "-", line, column - 1};
    }

    if (current == '*') {
        advance();
        return {TokenType::Star, "*", line, column - 1};
    }

    if (current == '/') {
        if (source[position + 1] == '/') {
            return lineComment();
        } else if (source[position + 1] == '*') {
            return blockComment();
        }
        advance();
        return {TokenType::Slash, "/", line, column - 1};
    }

    advance();
    return {TokenType::Unknown, std::string(1, current), line, column - 1};
}

Token Lexer::lineComment() {
    int start_line = line;
    int start_column = column;
    advance(); // Consume the first /
    advance(); // Consume the second /
    std::string value;
    while (currentChar() != '\0' && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    return {TokenType::LineComment, value, start_line, start_column};
}

Token Lexer::blockComment() {
    int start_line = line;
    int start_column = column;
    advance(); // Consume /
    advance(); // Consume *
    std::string value;
    while (currentChar() != '\0' && (currentChar() != '*' || source[position + 1] != '/')) {
        value += currentChar();
        advance();
    }
    advance(); // Consume *
    advance(); // Consume /
    return {TokenType::BlockComment, value, start_line, start_column};
}

Token Lexer::generatorComment() {
    int start_line = line;
    int start_column = column;
    advance(); // Consume #
    // Optional: skip a single space
    if (currentChar() == ' ') {
        advance();
    }
    std::string value;
    while (currentChar() != '\0' && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    return {TokenType::GeneratorComment, value, start_line, start_column};
}