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

        // Handle multi-character tokens and special single-character tokens first
        if (current == '/') {
            if (position + 1 < input.size()) {
                if (input[position + 1] == '/') return singleLineComment();
                if (input[position + 1] == '*') return multiLineComment();
            }
        }
        if (current == '*') {
            if (position + 1 < input.size() && input[position + 1] == '*') {
                advance();
                advance();
                return {TokenType::POWER, "**", line, column};
            }
        }
        if (current == '#') {
            if (position + 1 < input.size() && isspace(input[position + 1])) {
                return generatorComment();
            } else {
                return selector(); // ID selector
            }
        }
        if (current == '.') {
            if (position + 1 < input.size() && isdigit(input[position + 1])) {
                return number();
            }
             return selector(); // Class selector
        }
        if (current == '&') {
            if (position + 1 < input.size() && input[position + 1] == '&') {
                advance(); advance(); return {TokenType::LOGICAL_AND, "&&", line, column};
            }
            return contextualSelector();
        }
        if (current == '|') {
            if (position + 1 < input.size() && input[position + 1] == '|') {
                advance(); advance(); return {TokenType::LOGICAL_OR, "||", line, column};
            }
        }
        if (current == '=') {
            if (position + 1 < input.size() && input[position + 1] == '=') {
                advance(); advance(); return {TokenType::EQUAL_EQUAL, "==", line, column};
            }
            advance(); return {TokenType::EQUALS, "=", line, column};
        }
        if (current == '!') {
            if (position + 1 < input.size() && input[position + 1] == '=') {
                advance(); advance(); return {TokenType::NOT_EQUAL, "!=", line, column};
            }
        }
        if (current == '>') {
            if (position + 1 < input.size() && input[position + 1] == '=') {
                advance(); advance(); return {TokenType::GREATER_EQUAL, ">=", line, column};
            }
            advance(); return {TokenType::GREATER, ">", line, column};
        }
        if (current == '<') {
            if (position + 1 < input.size() && input[position + 1] == '=') {
                advance(); advance(); return {TokenType::LESS_EQUAL, "<=", line, column};
            }
            advance(); return {TokenType::LESS, "<", line, column};
        }

        // Handle standard single-character tokens
        switch (current) {
            case '{': advance(); return {TokenType::LEFT_BRACE, "{", line, column};
            case '}': advance(); return {TokenType::RIGHT_BRACE, "}", line, column};
            case '(': advance(); return {TokenType::LEFT_PAREN, "(", line, column};
            case ')': advance(); return {TokenType::RIGHT_PAREN, ")", line, column};
            case '+': advance(); return {TokenType::PLUS, "+", line, column};
            case '-': advance(); return {TokenType::MINUS, "-", line, column};
            case '/': advance(); return {TokenType::SLASH, "/", line, column};
            case '*': advance(); return {TokenType::STAR, "*", line, column};
            case '%': advance(); return {TokenType::PERCENT, "%", line, column};
            case '?': advance(); return {TokenType::QUESTION_MARK, "?", line, column};
            case ';': advance(); return {TokenType::SEMICOLON, ";", line, column};
            case ':': advance(); return {TokenType::COLON, ":", line, column};
        case ',': advance(); return {TokenType::COMMA, ",", line, column};
        }

        // Handle string literals
        if (current == '"' || current == '\'') {
            return stringLiteral();
        }

        if (isdigit(current)) {
            return number();
        }

        // If it's none of the above, it's an identifier
        if (isalnum(current) || current == '_') {
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

Token CHTLLexer::number() {
    std::string value;
    int startLine = line;
    int startColumn = column;

    // Consume digits before a decimal
    while(position < input.size() && isdigit(currentChar())) {
        value += currentChar();
        advance();
    }

    // Consume decimal and following digits
    if(position < input.size() && currentChar() == '.') {
        value += currentChar();
        advance();
        while(position < input.size() && isdigit(currentChar())) {
            value += currentChar();
            advance();
        }
    }

    // Consume unit
    while(position < input.size() && (isalpha(currentChar()) || currentChar() == '%')) {
        value += currentChar();
        advance();
    }

    return {TokenType::NUMBER, value, startLine, startColumn};
}

Token CHTLLexer::identifier() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    while (position < input.size() && (isalnum(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }

    // Check for property reference like "div.width"
    if (position < input.size() && currentChar() == '.') {
        value += currentChar();
        advance(); // consume '.'

        while (position < input.size() && (isalnum(currentChar()) || currentChar() == '_')) {
            value += currentChar();
            advance();
        }
        return {TokenType::PROPERTY_REFERENCE, value, startLine, startColumn};
    }

    if (value == "text") return {TokenType::KEYWORD_TEXT, value, startLine, startColumn};
    if (value == "style") return {TokenType::KEYWORD_STYLE, value, startLine, startColumn};
    if (value == "script") return {TokenType::KEYWORD_SCRIPT, value, startLine, startColumn};
    if (value == "if") return {TokenType::KEYWORD_IF, value, startLine, startColumn};
    if (value == "else") return {TokenType::KEYWORD_ELSE, value, startLine, startColumn};
    if (value == "condition") return {TokenType::KEYWORD_CONDITION, value, startLine, startColumn};

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
    std::string value;

    char selectorTypeChar = currentChar();
    value += selectorTypeChar;
    advance(); // Consume '.' or '#'

    while (position < input.size() && (isalnum(currentChar()) || currentChar() == '-')) {
        value += currentChar();
        advance();
    }

    // Check for property reference like ".box.width"
    if (position < input.size() && currentChar() == '.') {
        value += currentChar();
        advance(); // consume '.'

        while (position < input.size() && (isalnum(currentChar()) || currentChar() == '_')) {
            value += currentChar();
            advance();
        }
        return {TokenType::PROPERTY_REFERENCE, value, startLine, startColumn};
    }

    if (selectorTypeChar == '.') {
        return {TokenType::CLASS_SELECTOR, value.substr(1), startLine, startColumn};
    } else { // It must be '#'
        return {TokenType::ID_SELECTOR, value.substr(1), startLine, startColumn};
    }
}

Token CHTLLexer::contextualSelector() {
    int startLine = line;
    int startColumn = column;
    std::string value;

    // Consume the '&' and any following pseudo-class/element characters
    while (position < input.size() && (isalnum(currentChar()) || currentChar() == '&' || currentChar() == ':' || currentChar() == '-')) {
        value += currentChar();
        advance();
    }

    return {TokenType::CONTEXT_SELECTOR, value, startLine, startColumn};
}