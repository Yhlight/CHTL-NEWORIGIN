#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {
    keywords = {
        {"text", TokenType::Text},
        {"style", TokenType::Style},
        {"script", TokenType::Script},
        {"[Template]", TokenType::Template},
        {"[Custom]", TokenType::Custom},
        {"[Origin]", TokenType::Origin},
        {"[Import]", TokenType::Import},
        {"[Namespace]", TokenType::Namespace},
        {"[Configuration]", TokenType::Configuration},
        {"@Style", TokenType::AtStyle},
        {"@Element", TokenType::AtElement},
        {"@Var", TokenType::AtVar},
        {"@Html", TokenType::AtHtml},
        {"@JavaScript", TokenType::AtJavaScript},
        {"@Chtl", TokenType::AtChtl},
        {"@Config", TokenType::AtConfig},
        {"inherit", TokenType::Inherit},
        {"delete", TokenType::Delete},
        {"insert", TokenType::Insert},
        {"after", TokenType::After},
        {"before", TokenType::Before},
        {"replace", TokenType::Replace},
        {"top", TokenType::AtTop}, // "at top" is two tokens
        {"bottom", TokenType::AtBottom}, // "at bottom" is two tokens
        {"from", TokenType::From},
        {"as", TokenType::As},
        {"except", TokenType::Except},
        {"use", TokenType::Use},
        {"html5", TokenType::Html5}
    };
}

Token Lexer::getNextToken() {
    skipWhitespace();

    if (position >= source.length()) {
        return makeToken(TokenType::EndOfFile, "");
    }

    char current = currentChar();

    // In CHTL/CSS, identifiers can start with numbers (e.g., "100px").
    if (isalnum(current) || current == '@' || current == '_') {
        return handleIdentifierOrKeyword();
    }

    if (current == '[') {
        // Handle special keywords like [Template]
        if (source.substr(position, 10) == "[Template]") { position += 10; return makeToken(TokenType::Template, "[Template]"); }
        if (source.substr(position, 8) == "[Custom]") { position += 8; return makeToken(TokenType::Custom, "[Custom]"); }
        if (source.substr(position, 8) == "[Origin]") { position += 8; return makeToken(TokenType::Origin, "[Origin]"); }
        if (source.substr(position, 8) == "[Import]") { position += 8; return makeToken(TokenType::Import, "[Import]"); }
        if (source.substr(position, 11) == "[Namespace]") { position += 11; return makeToken(TokenType::Namespace, "[Namespace]"); }
        if (source.substr(position, 15) == "[Configuration]") { position += 15; return makeToken(TokenType::Configuration, "[Configuration]"); }
    }

    if (isdigit(current)) {
        // For now, we handle numbers as part of a more general "UnquotedLiteral"
        // A more robust solution would be a separate Number token
    }

    if (current == '"' || current == '\'') {
        return handleStringLiteral();
    }

    if (current == '#') {
        return handleGeneratorComment();
    }

    if (current == '/' && position + 1 < source.length()) {
        if (source[position + 1] == '/') {
            skipLineComment();
            return getNextToken(); // Get next token after comment
        }
        if (source[position + 1] == '*') {
            skipBlockComment();
            return getNextToken(); // Get next token after comment
        }
    }

    // For everything else, try to match operators or treat as unquoted literal
    return handleOperatorOrPunctuator();
}

std::vector<Token> Lexer::getAllTokens() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    while (token.type != TokenType::EndOfFile) {
        tokens.push_back(token);
        token = getNextToken();
    }
    tokens.push_back(token); // Add EOF token
    return tokens;
}


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
    while (position < source.length() && isspace(currentChar())) {
        advance();
    }
}

void Lexer::skipLineComment() {
    while (position < source.length() && currentChar() != '\n') {
        advance();
    }
}

void Lexer::skipBlockComment() {
    position += 2; // Skip "/*"
    while (position + 1 < source.length() && (currentChar() != '*' || source[position + 1] != '/')) {
        advance();
    }
    if (position + 1 < source.length()) {
        position += 2; // Skip "*/"
    } else {
        // Unterminated block comment, handle error or just stop
        position = source.length();
    }
}

Token Lexer::handleGeneratorComment() {
    int startCol = column;
    advance(); // Skip '#'
    std::string value;
    while (position < source.length() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    return {TokenType::GeneratorComment, value, line, startCol};
}

Token Lexer::handleStringLiteral() {
    int startCol = column;
    char quoteType = currentChar();
    advance(); // Skip opening quote
    std::string value;
    while (position < source.length() && currentChar() != quoteType) {
        // Handle escape sequences if necessary
        if (currentChar() == '\\' && position + 1 < source.length()) {
            advance(); // Skip '\'
        }
        value += currentChar();
        advance();
    }
    advance(); // Skip closing quote
    return {TokenType::String, value, line, startCol};
}

Token Lexer::handleIdentifierOrKeyword() {
    int startCol = column;
    std::string value;
    // For @keywords
    if (currentChar() == '@') {
        value += currentChar();
        advance();
    }
    while (position < source.length() && (isalnum(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return {it->second, value, line, startCol};
    }
    // CHTL allows unquoted literals which can look like identifiers.
    // The parser will determine the context. Here we will classify them as Identifier
    // for things that look like variable names, and UnquotedLiteral for others.
    // For simplicity now, let's call them all identifiers.
    return {TokenType::Identifier, value, line, startCol};
}


Token Lexer::handleOperatorOrPunctuator() {
    int startCol = column;
    char current = currentChar();

    // Two-character operators first
    if (position + 1 < source.length()) {
        char next = source[position + 1];
        if (current == '*' && next == '*') {
            advance(); advance();
            return {TokenType::DoubleAsterisk, "**", line, startCol};
        }
        if (current == '&' && next == '&') {
            advance(); advance();
            return {TokenType::DoubleAmpersand, "&&", line, startCol};
        }
        if (current == '|' && next == '|') {
            advance(); advance();
            return {TokenType::DoublePipe, "||", line, startCol};
        }
    }

    // One-character operators and punctuators
    advance();
    switch (current) {
        case '{': return {TokenType::OpenBrace, "{", line, startCol};
        case '}': return {TokenType::CloseBrace, "}", line, startCol};
        case '(': return {TokenType::OpenParen, "(", line, startCol};
        case ')': return {TokenType::CloseParen, ")", line, startCol};
        case '[': return {TokenType::OpenBracket, "[", line, startCol};
        case ']': return {TokenType::CloseBracket, "]", line, startCol};
        case ':': return {TokenType::Colon, ":", line, startCol};
        case ';': return {TokenType::Semicolon, ";", line, startCol};
        case '=': return {TokenType::Equals, "=", line, startCol};
        case ',': return {TokenType::Comma, ",", line, startCol};
        case '.': return {TokenType::Dot, ".", line, startCol};
        case '+': return {TokenType::Plus, "+", line, startCol};
        case '-': return {TokenType::Minus, "-", line, startCol};
        case '*': return {TokenType::Asterisk, "*", line, startCol};
        case '/': return {TokenType::Slash, "/", line, startCol};
        case '%': return {TokenType::Percent, "%", line, startCol};
        case '?': return {TokenType::QuestionMark, "?", line, startCol};
        case '&': return {TokenType::Ampersand, "&", line, startCol};
        default:
            // This is likely an unquoted literal that doesn't start with a letter.
            // For now, we'll treat it as an unexpected character.
            // A more sophisticated approach is needed for unquoted literals.
            return {TokenType::Unexpected, std::string(1, current), line, startCol};
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, line, column};
}

Token Lexer::makeToken(TokenType type, char value) {
    return {type, std::string(1, value), line, column};
}
