#include "CHTLLexer.h"
#include <cctype>
#include <string>

namespace CHTL {

    CHTLLexer::CHTLLexer(const std::string& source)
        : source(source), position(0), line(1), column(1) {}

    void CHTLLexer::advance() {
        if (position < source.length()) {
            if (source[position] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            position++;
        }
    }

    char CHTLLexer::currentChar() {
        if (position >= source.length()) {
            return '\0'; // End of file
        }
        return source[position];
    }

    char CHTLLexer::peek() {
        if (position + 1 >= source.length()) {
            return '\0';
        }
        return source[position + 1];
    }

    void CHTLLexer::skipWhitespace() {
        while (currentChar() != '\0' && std::isspace(currentChar())) {
            advance();
        }
    }

    void CHTLLexer::skipSingleLineComment() {
        while (currentChar() != '\0' && currentChar() != '\n') {
            advance();
        }
    }

    void CHTLLexer::skipMultiLineComment() {
        // Assumes we have just consumed '/*'
        while (currentChar() != '\0') {
            if (currentChar() == '*' && peek() == '/') {
                advance(); // consume '*'
                advance(); // consume '/'
                return;
            }
            advance();
        }
        // In a real implementation, we'd handle unterminated comment errors.
    }

    Token CHTLLexer::generatorComment() {
        int startCol = column;
        advance(); // consume '#'

        if (currentChar() != ' ') {
            // Not a valid generator comment, treat '#' as an unknown token.
            return {TokenType::UNKNOWN, "#", line, startCol};
        }
        advance(); // consume space

        std::string value;
        while (currentChar() != '\0' && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        return {TokenType::GENERATOR_COMMENT, value, line, startCol};
    }

    Token CHTLLexer::identifier() {
        std::string value;
        int startCol = column;
        // Added '#' to separators
        while (currentChar() != '\0' && !std::isspace(currentChar()) &&
               std::string("{}():;=,#").find(currentChar()) == std::string::npos) {
            value += currentChar();
            advance();
        }

        return {TokenType::IDENTIFIER, value, line, startCol};
    }

    Token CHTLLexer::stringLiteral(char quoteType) {
        std::string value;
        int startCol = column;
        advance(); // consume opening quote
        while (currentChar() != '\0' && currentChar() != quoteType) {
            value += currentChar();
            advance();
        }
        if(currentChar() == quoteType) {
            advance(); // consume closing quote
        }
        return {TokenType::STRING_LITERAL, value, line, startCol};
    }

    Token CHTLLexer::numberLiteral() {
        std::string value;
        int startCol = column;
        while (currentChar() != '\0' && std::isdigit(currentChar())) {
            value += currentChar();
            advance();
        }
        return {TokenType::NUMERIC_LITERAL, value, line, startCol};
    }

    Token CHTLLexer::getNextToken() {
        while (currentChar() != '\0') {
            if (std::isspace(currentChar())) {
                skipWhitespace();
                continue;
            }

            // Check for comments
            if (currentChar() == '/') {
                if (peek() == '/') {
                    advance(); // consume first '/'
                    advance(); // consume second '/'
                    skipSingleLineComment();
                    continue;
                } else if (peek() == '*') {
                    advance(); // consume '/'
                    advance(); // consume '*'
                    skipMultiLineComment();
                    continue;
                }
            }

            if (currentChar() == '#') {
                return generatorComment();
            }

            if (currentChar() == '"' || currentChar() == '\'') {
                return stringLiteral(currentChar());
            }

            int startCol = column;
            char char_val = currentChar();

            switch (char_val) {
                case '{': advance(); return {TokenType::LEFT_BRACE, "{", line, startCol};
                case '}': advance(); return {TokenType::RIGHT_BRACE, "}", line, startCol};
                case '(': advance(); return {TokenType::LEFT_PAREN, "(", line, startCol};
                case ')': advance(); return {TokenType::RIGHT_PAREN, ")", line, startCol};
                case ':': advance(); return {TokenType::COLON, ":", line, startCol};
                case ';': advance(); return {TokenType::SEMICOLON, ";", line, startCol};
                case '=': advance(); return {TokenType::ASSIGN, "=", line, startCol};
                case ',': advance(); return {TokenType::COMMA, ",", line, startCol};
            }

            if (std::isdigit(currentChar())) {
                return numberLiteral();
            }

            if (!std::isspace(currentChar()) && std::string("{}():;=,#").find(currentChar()) == std::string::npos) {
                 return identifier();
            }

            advance();
            return {TokenType::UNKNOWN, std::string(1, char_val), line, startCol};
        }

        return {TokenType::END_OF_FILE, "", line, column};
    }
}
