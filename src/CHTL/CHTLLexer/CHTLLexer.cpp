#include "CHTLLexer.h"
#include <cctype>
#include <iostream>

CHTLLexer::CHTLLexer(const std::string& input)
    : input(input), rawContentMode(false), position(0), line(1), column(1) {}

void CHTLLexer::setRawContentMode(bool enabled) {
    rawContentMode = enabled;
}

Token CHTLLexer::getNextToken() {
    if (rawContentMode) {
        skipWhitespace();
        if (currentChar() == '{') {
            advance(); // Consume '{'
            std::string content;
            int braceCount = 1;
            while (position < input.size() && braceCount > 0) {
                if (currentChar() == '{') {
                    braceCount++;
                } else if (currentChar() == '}') {
                    braceCount--;
                }
                if (braceCount > 0) {
                    content += currentChar();
                }
                advance();
            }
            rawContentMode = false; // Reset raw content mode
            return {TokenType::STRING_LITERAL, content, line, column};
        }
    }

    while (position < input.size()) {
        char current = currentChar();

        if (isspace(current)) {
            skipWhitespace();
            continue;
        }

        // Handle selectors
        if (current == '.') { // Class selector
            return selector();
        }
        if (current == '#') { // ID selector OR Generator Comment
            if (position + 1 < input.size() && isspace(input[position + 1])) {
                return generatorComment();
            } else {
                return selector();
            }
        }

        // Handle single-character tokens
        switch (current) {
            case '{': advance(); return {TokenType::LEFT_BRACE, "{", line, column};
            case '}': advance(); return {TokenType::RIGHT_BRACE, "}", line, column};
            case '(': advance(); return {TokenType::LEFT_PAREN, "(", line, column};
            case ')': advance(); return {TokenType::RIGHT_PAREN, ")", line, column};
            case ';': advance(); return {TokenType::SEMICOLON, ";", line, column};
            case ':': advance(); return {TokenType::COLON, ":", line, column};
            case '=': advance(); return {TokenType::EQUALS, "=", line, column};
        }

        // Handle multi-character comments
        if (current == '/') {
            if (position + 1 < input.size()) {
                if (input[position + 1] == '/') return singleLineComment();
                if (input[position + 1] == '*') return multiLineComment();
            }
        }

        // Handle string literals
        if (current == '"' || current == '\'') {
            return stringLiteral();
        }

        // If it's none of the above, it's an identifier
        if (isgraph(current)) {
            return identifier();
        }

        // If we reach here, it's an unknown character.
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
    // Consume characters until a whitespace or a CHTL punctuation character is found.
    while (position < input.size() && !isspace(currentChar()) && currentChar() != '{' && currentChar() != '}' && currentChar() != ';' && currentChar() != ':' && currentChar() != '=' && currentChar() != '(' && currentChar() != ')' && currentChar() != '.' && currentChar() != '#') {
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

Token CHTLLexer::selector() {
    int startLine = line;
    int startColumn = column;
    char selectorTypeChar = currentChar();
    advance(); // Consume '.' or '#'

    std::string value;
    while (position < input.size() && (isalnum(currentChar()) || currentChar() == '-')) {
        value += currentChar();
        advance();
    }

    if (selectorTypeChar == '.') {
        return {TokenType::CLASS_SELECTOR, value, startLine, startColumn};
    } else { // It must be '#'
        return {TokenType::ID_SELECTOR, value, startLine, startColumn};
    }
}