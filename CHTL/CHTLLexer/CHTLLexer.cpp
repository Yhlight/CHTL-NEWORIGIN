#include "CHTLLexer.h"
#include <cctype>
#include <unordered_map>

const std::string& CHTLLexer::getSource() const {
    return source;
}

size_t CHTLLexer::getPosition() const {
    return pos;
}

void CHTLLexer::setPosition(size_t newPos) {
    pos = newPos;
    // Note: This simplified implementation doesn't update line/column counts.
    // This is acceptable for the raw parsing feature, which will immediately
    // re-sync the parser by calling consumeToken().
}

CHTLLexer::CHTLLexer(const std::string& source)
    : source(source), pos(0), line(1), column(1), isPeeked(false) {}

Token CHTLLexer::getNextToken() {
    if (isPeeked) {
        isPeeked = false;
        return peekBuffer;
    }
    return generateNextToken();
}

Token CHTLLexer::peekToken() {
    if (isPeeked) {
        return peekBuffer;
    }
    peekBuffer = generateNextToken();
    isPeeked = true;
    return peekBuffer;
}

char CHTLLexer::currentChar() {
    if (pos >= source.length()) {
        return '\0'; // End of file
    }
    return source[pos];
}

char CHTLLexer::peekChar() {
    if (pos + 1 >= source.length()) {
        return '\0';
    }
    return source[pos + 1];
}

void CHTLLexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
}

void CHTLLexer::skipWhitespace() {
    while (currentChar() != '\0' && isspace(currentChar())) {
        advance();
    }
}

void CHTLLexer::skipLineComment() {
    while (currentChar() != '\0' && currentChar() != '\n') {
        advance();
    }
}

void CHTLLexer::skipBlockComment() {
    advance(); // Consume '*'
    advance(); // Consume '/'
    while (currentChar() != '\0' && (currentChar() != '*' || peekChar() != '/')) {
        advance();
    }
    if (currentChar() == '*') advance(); // Consume '*'
    if (currentChar() == '/') advance(); // Consume '/'
}

Token CHTLLexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, line, column};
}

Token CHTLLexer::identifier() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"Style", TokenType::KEYWORD_STYLE},
        {"script", TokenType::KEYWORD_SCRIPT},
        {"Template", TokenType::KEYWORD_TEMPLATE},
        {"Element", TokenType::KEYWORD_ELEMENT},
        {"Var", TokenType::KEYWORD_VAR},
        {"Import", TokenType::KEYWORD_IMPORT},
        {"Custom", TokenType::KEYWORD_CUSTOM},
        {"Origin", TokenType::KEYWORD_ORIGIN},
        {"Namespace", TokenType::KEYWORD_NAMESPACE},
        {"use", TokenType::KEYWORD_USE},
        {"html5", TokenType::KEYWORD_HTML5},
        {"Html", TokenType::KEYWORD_HTML},
        {"JavaScript", TokenType::KEYWORD_JAVASCRIPT},
        {"from", TokenType::KEYWORD_FROM},
        {"as", TokenType::KEYWORD_AS},
        {"inherit", TokenType::KEYWORD_INHERIT}
    };

    std::string value;
    size_t startCol = column;
    // Allow hyphens in identifiers, but not at the start
    while (currentChar() != '\0' && (isalnum(currentChar()) || currentChar() == '_' || (currentChar() == '-' && !value.empty()))) {
        value += currentChar();
        advance();
    }

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return {it->second, value, line, startCol};
    }

    return {TokenType::IDENTIFIER, value, line, startCol};
}

Token CHTLLexer::number() {
    std::string value;
    size_t startCol = column;
    bool has_dot = false;
    while (currentChar() != '\0' && (isdigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (has_dot) break; // only one dot allowed
            has_dot = true;
        }
        value += currentChar();
        advance();
    }
    return {TokenType::NUMBER, value, line, startCol};
}


Token CHTLLexer::stringLiteral(char quote) {
    std::string value;
    size_t startCol = column;
    advance(); // Consume opening quote
    while (currentChar() != '\0' && currentChar() != quote) {
        // Handle escape sequences if necessary, for now, just consume
        value += currentChar();
        advance();
    }
    advance(); // Consume closing quote
    return {TokenType::STRING, value, line, startCol};
}


Token CHTLLexer::generateNextToken() {
    skipWhitespace();

    char current = currentChar();
    if (current == '\0') return makeToken(TokenType::END_OF_FILE, "");

    // Identifiers and Keywords
    if (isalpha(current) || current == '_') {
        return identifier();
    }

    // Numbers
    if (isdigit(current)) {
        return number();
    }

    // String literals
    if (current == '"' || current == '\'') {
        return stringLiteral(current);
    }

    // Comments and Operators
    if (current == '/') {
        if (peekChar() == '/') {
            skipLineComment();
            return generateNextToken(); // Get next token after comment
        }
        if (peekChar() == '*') {
            skipBlockComment();
            return generateNextToken(); // Get next token after comment
        }
        advance();
        return makeToken(TokenType::SLASH, "/");
    }

    if (current == '*') {
        if (peekChar() == '*') {
            advance();
            advance();
            return makeToken(TokenType::DOUBLE_STAR, "**");
        }
        advance();
        return makeToken(TokenType::STAR, "*");
    }

    if (current == '=') {
        if (peekChar() == '=') {
            advance();
            advance();
            return makeToken(TokenType::EQUAL_EQUAL, "==");
        }
        advance();
        return makeToken(TokenType::EQUAL, "=");
    }

    if (current == '!') {
        if (peekChar() == '=') {
            advance();
            advance();
            return makeToken(TokenType::NOT_EQUAL, "!=");
        }
        // Add ! as a logical NOT operator later if needed
    }

    if (current == '&') {
        if (peekChar() == '&') {
            advance();
            advance();
            return makeToken(TokenType::LOGICAL_AND, "&&");
        }
    }

    if (current == '|') {
        if (peekChar() == '|') {
            advance();
            advance();
            return makeToken(TokenType::LOGICAL_OR, "||");
        }
    }

    if (current == '#') {
        if (peekChar() == ' ') {
            // It's a generator comment
            advance(); // consume '#'
            advance(); // consume ' '
            std::string comment_value;
            while (currentChar() != '\0' && currentChar() != '\n') {
                comment_value += currentChar();
                advance();
            }
            return makeToken(TokenType::HASH_COMMENT, comment_value);
        } else if (isxdigit(peekChar())) {
            // It's a hex color code
            std::string hex_value;
            hex_value += currentChar();
            advance();
            while (isxdigit(currentChar())) {
                hex_value += currentChar();
                advance();
            }
            return makeToken(TokenType::STRING, hex_value);
        }
        else {
            // It's an ID selector token
            advance();
            return makeToken(TokenType::HASH, "#");
        }
    }

    // Single-character symbols
    switch (current) {
        case '.': advance(); return makeToken(TokenType::DOT, ".");
        case '&': advance(); return makeToken(TokenType::AMPERSAND, "&");
        case '{': advance(); return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': advance(); return makeToken(TokenType::RIGHT_BRACE, "}");
        case '(': advance(); return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': advance(); return makeToken(TokenType::RIGHT_PAREN, ")");
        case '[': advance(); return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': advance(); return makeToken(TokenType::RIGHT_BRACKET, "]");
        case ':': advance(); return makeToken(TokenType::COLON, ":");
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";");
        case ',': advance(); return makeToken(TokenType::COMMA, ",");
        case '+': advance(); return makeToken(TokenType::PLUS, "+");
        case '-': advance(); return makeToken(TokenType::MINUS, "-");
        case '%': advance(); return makeToken(TokenType::PERCENT, "%");
        case '@': advance(); return makeToken(TokenType::AT, "@");
        case '>': advance(); return makeToken(TokenType::GREATER, ">");
        case '<': advance(); return makeToken(TokenType::LESS, "<");
        case '?': advance(); return makeToken(TokenType::QUESTION_MARK, "?");
    }

    // If nothing matches, it's an unknown character
    std::string unknown_char;
    unknown_char += current;
    advance();
    return makeToken(TokenType::UNKNOWN, unknown_char);
}
