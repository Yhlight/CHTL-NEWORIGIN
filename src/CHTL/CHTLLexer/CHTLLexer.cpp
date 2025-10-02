#include "CHTLLexer.h"
#include <cctype>
#include <iostream>

CHTLLexer::CHTLLexer(const std::string& input)
    : input(input), position(0), line(1), column(1) {}

Token CHTLLexer::getNextToken() {
    while (position < input.size()) {
        char current = currentChar();

        if (isspace(current)) {
            skipWhitespace();
            continue;
        }

        if (isalpha(current) || current == '_') {
            return identifier();
        }

        if (current == '"' || current == '\'') {
            return stringLiteral();
        }

        if (current == '/') {
            if (position + 1 < input.size()) {
                if (input[position + 1] == '/') {
                    return singleLineComment();
                } else if (input[position + 1] == '*') {
                    return multiLineComment();
                }
            }
        }

        if (current == '#') {
            return generatorComment();
        }

        switch (current) {
            case '{':
                advance();
                return {TokenType::LEFT_BRACE, "{", line, column};
            case '}':
                advance();
                return {TokenType::RIGHT_BRACE, "}", line, column};
            case ';':
                advance();
                return {TokenType::SEMICOLON, ";", line, column};
            case ':':
                advance();
                return {TokenType::COLON, ":", line, column};
            case '=':
                advance();
                return {TokenType::EQUALS, "=", line, column};
        }

        // For now, any other character is considered part of an unquoted literal
        if (isgraph(current)) {
            return unquotedLiteral();
        }


        // If we reach here, it's an unknown character. Advance past it.
        advance();
        return {TokenType::UNKNOWN, std::string(1, current), line, column};
    }

    return {TokenType::END_OF_FILE, "", line, column};
}

char CHTLLexer::currentChar() {
    if (position < input.size()) {
        return input[position];
    }
    return '\0';
}

void CHTLLexer::advance() {
    if (position < input.size()) {
        if (input[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void CHTLLexer::skipWhitespace() {
    while (position < input.size() && isspace(currentChar())) {
        advance();
    }
}

Token CHTLLexer::identifier() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    while (position < input.size() && (isalnum(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }

    if (value == "text") return {TokenType::KEYWORD_TEXT, value, startLine, startColumn};
    if (value == "style") return {TokenType::KEYWORD_STYLE, value, startLine, startColumn};
    if (value == "script") return {TokenType::KEYWORD_SCRIPT, value, startLine, startColumn};

    return {TokenType::IDENTIFIER, value, startLine, startColumn};
}

Token CHTLLexer::stringLiteral() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    char quoteType = currentChar();
    advance(); // Consume opening quote

    while (position < input.size() && currentChar() != quoteType) {
        value += currentChar();
        advance();
    }

    advance(); // Consume closing quote
    return {TokenType::STRING_LITERAL, value, startLine, startColumn};
}

Token CHTLLexer::unquotedLiteral() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    while (position < input.size() && isgraph(currentChar()) && currentChar() != '{' && currentChar() != '}' && currentChar() != ';' && currentChar() != ':' && currentChar() != '=') {
        value += currentChar();
        advance();
    }
    return {TokenType::UNQUOTED_LITERAL, value, startLine, startColumn};
}

Token CHTLLexer::singleLineComment() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    advance(); // Consume first '/'
    advance(); // Consume second '/'
    while (position < input.size() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    return {TokenType::SINGLE_LINE_COMMENT, value, startLine, startColumn};
}

Token CHTLLexer::multiLineComment() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    advance(); // Consume '/'
    advance(); // Consume '*'
    while (position + 1 < input.size() && !(currentChar() == '*' && input[position + 1] == '/')) {
        value += currentChar();
        advance();
    }
    advance(); // Consume '*'
    advance(); // Consume '/'
    return {TokenType::MULTI_LINE_COMMENT, value, startLine, startColumn};
}

Token CHTLLexer::generatorComment() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    advance(); // Consume '#'
    while (position < input.size() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    return {TokenType::GENERATOR_COMMENT, value, startLine, startColumn};
}

// This function is not called from getNextToken, so it is dead code.
// I am removing it to avoid confusion.
// Token CHTLLexer::comment() {
//     // Placeholder implementation
//     return {TokenType::UNKNOWN, "", line, column};
// }