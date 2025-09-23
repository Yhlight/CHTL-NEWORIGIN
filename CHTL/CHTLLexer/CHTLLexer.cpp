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
    : source(source), pos(0), line(1), column(1), isPeeked(false), is_marked(false) {}

void CHTLLexer::markPosition() {
    marked_pos = pos;
    marked_line = line;
    marked_column = column;
    is_marked = true;
    // Note: This simple implementation doesn't save/restore the peekBuffer state,
    // so the parser must not use peekToken() during a lookahead scan.
}

void CHTLLexer::rewindToMark() {
    if (is_marked) {
        pos = marked_pos;
        line = marked_line;
        column = marked_column;
        is_marked = false;
        isPeeked = false; // invalidate peek buffer after a rewind
    }
}

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

Token CHTLLexer::makeToken(TokenType type, const std::string& value, size_t token_pos, size_t token_col) {
    return {type, value, line, token_col, token_pos};
}

Token CHTLLexer::identifier(size_t start_pos, size_t start_col) {
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
    // Allow hyphens in identifiers, but not at the start
    while (currentChar() != '\0' && (isalnum(currentChar()) || currentChar() == '_' || (currentChar() == '-' && !value.empty()))) {
        value += currentChar();
        advance();
    }

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return makeToken(it->second, value, start_pos, start_col);
    }

    return makeToken(TokenType::IDENTIFIER, value, start_pos, start_col);
}

Token CHTLLexer::number(size_t start_pos, size_t start_col) {
    std::string value;
    bool has_dot = false;
    while (currentChar() != '\0' && (isdigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (has_dot) break; // only one dot allowed
            has_dot = true;
        }
        value += currentChar();
        advance();
    }
    return makeToken(TokenType::NUMBER, value, start_pos, start_col);
}


Token CHTLLexer::stringLiteral(char quote, size_t start_pos, size_t start_col) {
    std::string value;
    advance(); // Consume opening quote
    while (currentChar() != '\0' && currentChar() != quote) {
        // Handle escape sequences if necessary, for now, just consume
        value += currentChar();
        advance();
    }
    advance(); // Consume closing quote
    return makeToken(TokenType::STRING, value, start_pos, start_col);
}


Token CHTLLexer::generateNextToken() {
    skipWhitespace();

    size_t start_pos = pos;
    size_t start_col = column;
    char current = currentChar();
    if (current == '\0') return makeToken(TokenType::END_OF_FILE, "", start_pos, start_col);

    // Identifiers and Keywords
    if (isalpha(current) || current == '_') {
        return identifier(start_pos, start_col);
    }

    // Numbers
    if (isdigit(current)) {
        return number(start_pos, start_col);
    }

    // String literals
    if (current == '"' || current == '\'') {
        return stringLiteral(current, start_pos, start_col);
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
        return makeToken(TokenType::SLASH, "/", start_pos, start_col);
    }

    if (current == '*') {
        if (peekChar() == '*') {
            advance();
            advance();
            return makeToken(TokenType::DOUBLE_STAR, "**", start_pos, start_col);
        }
        advance();
        return makeToken(TokenType::STAR, "*", start_pos, start_col);
    }

    if (current == '=') {
        if (peekChar() == '=') {
            advance();
            advance();
            return makeToken(TokenType::EQUAL_EQUAL, "==", start_pos, start_col);
        }
        advance();
        return makeToken(TokenType::EQUAL, "=", start_pos, start_col);
    }

    if (current == '!') {
        if (peekChar() == '=') {
            advance();
            advance();
            return makeToken(TokenType::NOT_EQUAL, "!=", start_pos, start_col);
        }
        // Add ! as a logical NOT operator later if needed
    }

    if (current == '&') {
        if (peekChar() == '&') {
            advance();
            advance();
            return makeToken(TokenType::LOGICAL_AND, "&&", start_pos, start_col);
        }
    }

    if (current == '|') {
        if (peekChar() == '|') {
            advance();
            advance();
            return makeToken(TokenType::LOGICAL_OR, "||", start_pos, start_col);
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
            return makeToken(TokenType::HASH_COMMENT, comment_value, start_pos, start_col);
        } else {
            // It's an ID selector
            advance();
            return makeToken(TokenType::HASH, "#", start_pos, start_col);
        }
    }

    // Single-character symbols
    switch (current) {
        case '.': advance(); return makeToken(TokenType::DOT, ".", start_pos, start_col);
        case '&': advance(); return makeToken(TokenType::AMPERSAND, "&", start_pos, start_col);
        case '{': advance(); return makeToken(TokenType::LEFT_BRACE, "{", start_pos, start_col);
        case '}': advance(); return makeToken(TokenType::RIGHT_BRACE, "}", start_pos, start_col);
        case '(': advance(); return makeToken(TokenType::LEFT_PAREN, "(", start_pos, start_col);
        case ')': advance(); return makeToken(TokenType::RIGHT_PAREN, ")", start_pos, start_col);
        case '[': advance(); return makeToken(TokenType::LEFT_BRACKET, "[", start_pos, start_col);
        case ']': advance(); return makeToken(TokenType::RIGHT_BRACKET, "]", start_pos, start_col);
        case ':': advance(); return makeToken(TokenType::COLON, ":", start_pos, start_col);
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";", start_pos, start_col);
        case ',': advance(); return makeToken(TokenType::COMMA, ",", start_pos, start_col);
        case '+': advance(); return makeToken(TokenType::PLUS, "+", start_pos, start_col);
        case '-': advance(); return makeToken(TokenType::MINUS, "-", start_pos, start_col);
        case '%': advance(); return makeToken(TokenType::PERCENT, "%", start_pos, start_col);
        case '@': advance(); return makeToken(TokenType::AT, "@", start_pos, start_col);
        case '>': advance(); return makeToken(TokenType::GREATER, ">", start_pos, start_col);
        case '<': advance(); return makeToken(TokenType::LESS, "<", start_pos, start_col);
        case '?': advance(); return makeToken(TokenType::QUESTION_MARK, "?", start_pos, start_col);
    }

    // If nothing matches, it's an unknown character
    std::string unknown_char;
    unknown_char += current;
    advance();
    return makeToken(TokenType::UNKNOWN, unknown_char, start_pos, start_col);
}
