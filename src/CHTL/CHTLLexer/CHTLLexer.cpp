#include "CHTLLexer.h"
#include <iostream>
#include <fstream>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() : source(""), filename(""), position(0) {}

CHTLLexer::CHTLLexer(const String& src, const String& file)
    : source(src), filename(file), position(0), line(1), column(1),
      inComment(false), inString(false), stringDelimiter('\0'), inGeneratorComment(false) {
    
    initializeKeywords();
    initializeOperators();
    initializeBrackets();
    initializeRegex();
}

void CHTLLexer::initializeKeywords() {
    keywords = {
        {"text", "TEXT"},
        {"style", "STYLE"},
        {"script", "SCRIPT"},
        {"template", "TEMPLATE"},
        {"custom", "CUSTOM"},
        {"origin", "ORIGIN"},
        {"import", "IMPORT"},
        {"namespace", "NAMESPACE"},
        {"configuration", "CONFIGURATION"},
        {"inherit", "INHERIT"},
        {"delete", "DELETE"},
        {"insert", "INSERT"},
        {"from", "FROM"},
        {"as", "AS"},
        {"except", "EXCEPT"},
        {"use", "USE"},
        {"html5", "HTML5"},
        {"true", "TRUE"},
        {"false", "FALSE"},
        {"null", "NULL"}
    };
}

void CHTLLexer::initializeOperators() {
    operators = {
        {":", "COLON"},
        {"=", "EQUAL"},
        {"+", "PLUS"},
        {"-", "MINUS"},
        {"*", "MULTIPLY"},
        {"/", "DIVIDE"},
        {"%", "MODULO"},
        {"**", "POWER"},
        {"==", "EQUAL_TO"},
        {"!=", "NOT_EQUAL"},
        {"<", "LESS_THAN"},
        {">", "GREATER_THAN"},
        {"<=", "LESS_EQUAL"},
        {">=", "GREATER_EQUAL"},
        {"&&", "AND"},
        {"||", "OR"},
        {"!", "NOT"},
        {"?", "QUESTION_MARK"},
        {";", "SEMICOLON"},
        {",", "COMMA"},
        {".", "DOT"},
        {"->", "ARROW"}
    };
}

void CHTLLexer::initializeBrackets() {
    brackets = {
        {"{", "LEFT_BRACE"},
        {"}", "RIGHT_BRACE"},
        {"(", "LEFT_PAREN"},
        {")", "RIGHT_PAREN"},
        {"[", "LEFT_BRACKET"},
        {"]", "RIGHT_BRACKET"}
    };
}

void CHTLLexer::initializeRegex() {
    identifierRegex = std::regex("^[a-zA-Z_][a-zA-Z0-9_]*");
    numberRegex = std::regex("^[0-9]+\\.?[0-9]*");
    stringRegex = std::regex("^\"([^\"\\\\]|\\\\.)*\"|^'([^'\\\\]|\\\\.)*'");
    commentRegex = std::regex("^//.*|^/\\*.*?\\*/");
    generatorCommentRegex = std::regex("^#\\s+.*");
}

TokenVector CHTLLexer::tokenize() {
    tokens.clear();
    reset();
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        char c = currentChar();
        
        // Handle different token types
        if (isAlpha(c) || c == '_') {
            tokens.push_back(scanIdentifier());
        }
        else if (c == '"' || c == '\'') {
            tokens.push_back(scanString());
        }
        else if (isDigit(c)) {
            tokens.push_back(scanNumber());
        }
        else if (c == '/' && (peekChar() == '/' || peekChar() == '*')) {
            tokens.push_back(scanComment());
        }
        else if (c == '#') {
            tokens.push_back(scanGeneratorComment());
        }
        else if (isBracketChar(c)) {
            tokens.push_back(scanBracket());
        }
        else if (isOperatorChar(c)) {
            tokens.push_back(scanOperator());
        }
        else if (c == '@') {
            advance();
            tokens.push_back(createToken(TokenType::AT_SYMBOL, "@"));
        }
        else if (c == '$') {
            advance();
            tokens.push_back(createToken(TokenType::DOLLAR_SYMBOL, "$"));
        }
        else if (c == '&') {
            advance();
            tokens.push_back(createToken(TokenType::AMPERSAND_SYMBOL, "&"));
        }
        else {
            // Unknown character
            advance();
            tokens.push_back(createToken(TokenType::UNKNOWN, String(1, c)));
        }
    }
    
    tokens.push_back(createToken(TokenType::EOF_TOKEN, "EOF"));
    return tokens;
}

TokenVector CHTLLexer::tokenizeFile(const String& filepath) {
    String content = FileSystem::readFile(filepath);
    setSource(content, filepath);
    return tokenize();
}

char CHTLLexer::currentChar() const {
    if (position >= source.length()) return '\0';
    return source[position];
}

char CHTLLexer::peekChar(int offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

void CHTLLexer::advance(int steps) {
    for (int i = 0; i < steps; ++i) {
        if (position < source.length()) {
            updatePosition(source[position]);
            position++;
        }
    }
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        advance();
    }
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

std::shared_ptr<Token> CHTLLexer::scanIdentifier() {
    size_t start = position;
    String value;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()))) {
        value += currentChar();
        advance();
    }
    
    TokenType type = isKeyword(value) ? getKeywordType(value) : TokenType::IDENTIFIER;
    return createToken(type, value);
}

std::shared_ptr<Token> CHTLLexer::scanString() {
    size_t start = position;
    char delimiter = currentChar();
    String value;
    
    advance(); // Skip opening quote
    
    while (!isAtEnd() && currentChar() != delimiter) {
        if (currentChar() == '\\' && peekChar() != '\0') {
            advance(); // Skip backslash
            char escaped = currentChar();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += escaped; break;
            }
        } else {
            value += currentChar();
        }
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // Skip closing quote
    }
    
    return createToken(TokenType::STRING_LITERAL, value);
}

std::shared_ptr<Token> CHTLLexer::scanNumber() {
    size_t start = position;
    String value;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return createToken(TokenType::NUMBER_LITERAL, value);
}

std::shared_ptr<Token> CHTLLexer::scanComment() {
    size_t start = position;
    String value;
    
    if (peekChar() == '/') {
        // Single-line comment
        advance(2); // Skip //
        while (!isAtEnd() && !isNewline(currentChar())) {
            value += currentChar();
            advance();
        }
    } else if (peekChar() == '*') {
        // Multi-line comment
        advance(2); // Skip /*
        while (!isAtEnd() && !(currentChar() == '*' && peekChar() == '/')) {
            value += currentChar();
            advance();
        }
        if (!isAtEnd()) {
            advance(2); // Skip */
        }
    }
    
    return createToken(TokenType::COMMENT, value);
}

std::shared_ptr<Token> CHTLLexer::scanGeneratorComment() {
    size_t start = position;
    String value;
    
    advance(); // Skip #
    skipWhitespace(); // Skip space after #
    
    while (!isAtEnd() && !isNewline(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return createToken(TokenType::GENERATOR_COMMENT, value);
}

std::shared_ptr<Token> CHTLLexer::scanOperator() {
    String op;
    
    // Check for multi-character operators first
    if (currentChar() == '=' && peekChar() == '=') {
        op = "==";
        advance(2);
    } else if (currentChar() == '!' && peekChar() == '=') {
        op = "!=";
        advance(2);
    } else if (currentChar() == '<' && peekChar() == '=') {
        op = "<=";
        advance(2);
    } else if (currentChar() == '>' && peekChar() == '=') {
        op = ">=";
        advance(2);
    } else if (currentChar() == '&' && peekChar() == '&') {
        op = "&&";
        advance(2);
    } else if (currentChar() == '|' && peekChar() == '|') {
        op = "||";
        advance(2);
    } else if (currentChar() == '*' && peekChar() == '*') {
        op = "**";
        advance(2);
    } else if (currentChar() == '-' && peekChar() == '>') {
        op = "->";
        advance(2);
    } else {
        op = currentChar();
        advance();
    }
    
    TokenType type = isOperator(op) ? getOperatorType(op) : TokenType::UNKNOWN;
    return createToken(type, op);
}

std::shared_ptr<Token> CHTLLexer::scanBracket() {
    String bracket(1, currentChar());
    advance();
    
    TokenType type = isBracket(bracket) ? getBracketType(bracket) : TokenType::UNKNOWN;
    return createToken(type, bracket);
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c) || c == '_';
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n';
}

bool CHTLLexer::isOperatorChar(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' ||
           c == '|' || c == '?' || c == ':' || c == ';' || c == ',' ||
           c == '.' || c == '~' || c == '^';
}

bool CHTLLexer::isBracketChar(char c) const {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']';
}

bool CHTLLexer::isStringDelimiter(char c) const {
    return c == '"' || c == '\'';
}

bool CHTLLexer::isKeyword(const String& str) const {
    return keywords.find(str) != keywords.end();
}

bool CHTLLexer::isOperator(const String& str) const {
    return operators.find(str) != operators.end();
}

bool CHTLLexer::isBracket(const String& str) const {
    return brackets.find(str) != brackets.end();
}

TokenType CHTLLexer::getKeywordType(const String& str) const {
    auto it = keywords.find(str);
    if (it != keywords.end()) {
        return TokenUtil::stringToTokenType(it->second);
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getOperatorType(const String& str) const {
    auto it = operators.find(str);
    if (it != operators.end()) {
        return TokenUtil::stringToTokenType(it->second);
    }
    return TokenType::UNKNOWN;
}

TokenType CHTLLexer::getBracketType(const String& str) const {
    auto it = brackets.find(str);
    if (it != brackets.end()) {
        return TokenUtil::stringToTokenType(it->second);
    }
    return TokenType::UNKNOWN;
}

void CHTLLexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    inComment = false;
    inString = false;
    stringDelimiter = '\0';
    inGeneratorComment = false;
}

void CHTLLexer::setSource(const String& src, const String& file) {
    source = src;
    filename = file;
    reset();
}

std::shared_ptr<Token> CHTLLexer::createToken(TokenType type, const String& value) {
    return std::make_shared<Token>(type, value, line, column, filename);
}

void CHTLLexer::updatePosition(char c) {
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
}

void CHTLLexer::reportError(const String& message, int line, int col) {
    String error = "Lexer Error";
    if (!filename.empty()) {
        error += " in " + filename;
    }
    if (line > 0) {
        error += " at line " + std::to_string(line);
        if (col > 0) {
            error += ", column " + std::to_string(col);
        }
    }
    error += ": " + message;
    
    throw CHTLException(error, line, col);
}

} // namespace CHTL