#include "CHTLLexer.h"
#include <iostream>
#include <cctype>

CHTLLexer::CHTLLexer() 
    : currentPos_(0)
    , currentLine_(1)
    , currentColumn_(1)
    , debugMode_(false) {
    initializeKeywords();
}

bool CHTLLexer::tokenize(const std::string& input, std::vector<Token>& tokens) {
    input_ = input;
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    
    if (debugMode_) {
        std::cout << "开始词法分析，输入长度: " << input.length() << std::endl;
    }
    
    return scanTokens(tokens);
}

void CHTLLexer::initializeKeywords() {
    keywords_["[Template]"] = TokenType::TEMPLATE;
    keywords_["[Custom]"] = TokenType::CUSTOM;
    keywords_["[Origin]"] = TokenType::ORIGIN;
    keywords_["[Import]"] = TokenType::IMPORT;
    keywords_["[Configuration]"] = TokenType::CONFIGURATION;
    keywords_["[Namespace]"] = TokenType::NAMESPACE;
    keywords_["[Info]"] = TokenType::INFO;
    keywords_["[Export]"] = TokenType::EXPORT;
    
    // HTML元素关键字
    keywords_["html"] = TokenType::ELEMENT;
    keywords_["head"] = TokenType::ELEMENT;
    keywords_["body"] = TokenType::ELEMENT;
    keywords_["div"] = TokenType::ELEMENT;
    keywords_["span"] = TokenType::ELEMENT;
    keywords_["p"] = TokenType::ELEMENT;
    keywords_["a"] = TokenType::ELEMENT;
    keywords_["img"] = TokenType::ELEMENT;
    keywords_["button"] = TokenType::ELEMENT;
    keywords_["input"] = TokenType::ELEMENT;
    keywords_["form"] = TokenType::ELEMENT;
    keywords_["table"] = TokenType::ELEMENT;
    keywords_["tr"] = TokenType::ELEMENT;
    keywords_["td"] = TokenType::ELEMENT;
    keywords_["th"] = TokenType::ELEMENT;
    keywords_["ul"] = TokenType::ELEMENT;
    keywords_["ol"] = TokenType::ELEMENT;
    keywords_["li"] = TokenType::ELEMENT;
    keywords_["h1"] = TokenType::ELEMENT;
    keywords_["h2"] = TokenType::ELEMENT;
    keywords_["h3"] = TokenType::ELEMENT;
    keywords_["h4"] = TokenType::ELEMENT;
    keywords_["h5"] = TokenType::ELEMENT;
    keywords_["h6"] = TokenType::ELEMENT;
    
    // 特殊关键字
    keywords_["text"] = TokenType::ELEMENT;
    keywords_["style"] = TokenType::ELEMENT;
    keywords_["script"] = TokenType::ELEMENT;
    keywords_["@Style"] = TokenType::ATTRIBUTE;
    keywords_["@Element"] = TokenType::ATTRIBUTE;
    keywords_["@Var"] = TokenType::ATTRIBUTE;
    keywords_["@Html"] = TokenType::ATTRIBUTE;
    keywords_["@JavaScript"] = TokenType::ATTRIBUTE;
    keywords_["@Chtl"] = TokenType::ATTRIBUTE;
    keywords_["@CJmod"] = TokenType::ATTRIBUTE;
    keywords_["@Config"] = TokenType::ATTRIBUTE;
}

bool CHTLLexer::scanTokens(std::vector<Token>& tokens) {
    while (!isAtEnd()) {
        Token token = scanNextToken();
        tokens.push_back(token);
        
        if (token.type == TokenType::ERROR) {
            return false;
        }
        
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    if (debugMode_) {
        std::cout << "词法分析完成，共生成 " << tokens.size() << " 个token" << std::endl;
    }
    
    return true;
}

Token CHTLLexer::scanNextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", currentLine_, currentColumn_, 0);
    }
    
    char c = currentChar();
    
    // 检查注释
    if (c == '/' && peekChar() == '/') {
        return scanComment();
    }
    if (c == '/' && peekChar() == '*') {
        return scanComment();
    }
    
    // 检查生成器注释
    if (c == '#') {
        return scanGeneratorComment();
    }
    
    // 检查字符串
    if (c == '"' || c == '\'') {
        return scanString();
    }
    
    // 检查数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 检查标识符和关键字
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 检查运算符
    if (c == ':' || c == '=' || c == ';' || c == ',' || c == '.' || c == '/') {
        return scanOperator();
    }
    
    // 检查分隔符
    if (c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')') {
        return scanDelimiter();
    }
    
    // 检查特殊符号
    if (c == '@' || c == '#' || c == '&' || c == '$' || c == '?' || c == '!' || c == '_') {
        return scanSpecial();
    }
    
    // 检查文本内容
    if (c != '\n' && c != '\r') {
        return scanText();
    }
    
    // 未知字符
    return createErrorToken("未知字符: " + std::string(1, c));
}

Token CHTLLexer::scanIdentifier() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isAlphaNumeric(currentChar()) || currentChar() == '_' || currentChar() == ']') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    
    // 检查是否是关键字
    auto it = keywords_.find(value);
    if (it != keywords_.end()) {
        return Token(it->second, value, startLine, startColumn, value.length());
    }
    
    // 检查是否是HTML元素
    if (keywords_.find(value) != keywords_.end() && 
        keywords_[value] == TokenType::ELEMENT) {
        return Token(TokenType::ELEMENT, value, startLine, startColumn, value.length());
    }
    
    return Token(TokenType::IDENTIFIER, value, startLine, startColumn, value.length());
}

Token CHTLLexer::scanString() {
    char quote = currentChar();
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar() == quote) {
            advance(2); // 跳过转义字符
        } else {
            advance();
        }
    }
    
    if (isAtEnd()) {
        return createErrorToken("未闭合的字符串");
    }
    
    advance(); // 跳过结束引号
    
    std::string value = input_.substr(start + 1, currentPos_ - start - 2);
    return Token(TokenType::STRING, value, startLine, startColumn, value.length() + 2);
}

Token CHTLLexer::scanNumber() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isDigit(currentChar()) || currentChar() == '.') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return Token(TokenType::NUMBER, value, startLine, startColumn, value.length());
}

Token CHTLLexer::scanLiteral() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (!isAtEnd() && 
           !isWhitespace(currentChar()) && 
           currentChar() != ';' && 
           currentChar() != ':' && 
           currentChar() != '=' &&
           currentChar() != '{' &&
           currentChar() != '}' &&
           currentChar() != '[' &&
           currentChar() != ']' &&
           currentChar() != '(' &&
           currentChar() != ')' &&
           currentChar() != ',' &&
           currentChar() != '.') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return Token(TokenType::LITERAL, value, startLine, startColumn, value.length());
}

Token CHTLLexer::scanComment() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    if (peekChar() == '/') {
        // 单行注释
        advance(2);
        while (!isAtEnd() && !isNewline(currentChar())) {
            advance();
        }
    } else if (peekChar() == '*') {
        // 多行注释
        advance(2);
        while (!isAtEnd()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(2);
                break;
            }
            advance();
        }
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return Token(TokenType::COMMENT, value, startLine, startColumn, value.length());
}

Token CHTLLexer::scanGeneratorComment() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance(); // 跳过 #
    
    // 检查是否有空格
    if (currentChar() == ' ') {
        advance(); // 跳过空格
    }
    
    while (!isAtEnd() && !isNewline(currentChar())) {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, value.length());
}

Token CHTLLexer::scanOperator() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance();
    
    switch (c) {
        case ':': return Token(TokenType::COLON, ":", startLine, startColumn, 1);
        case '=': return Token(TokenType::EQUAL, "=", startLine, startColumn, 1);
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startColumn, 1);
        case ',': return Token(TokenType::COMMA, ",", startLine, startColumn, 1);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn, 1);
        case '/': return Token(TokenType::SLASH, "/", startLine, startColumn, 1);
        default: return createErrorToken("未知运算符: " + std::string(1, c));
    }
}

Token CHTLLexer::scanDelimiter() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance();
    
    switch (c) {
        case '{': return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn, 1);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn, 1);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn, 1);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn, 1);
        case '(': return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn, 1);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn, 1);
        default: return createErrorToken("未知分隔符: " + std::string(1, c));
    }
}

Token CHTLLexer::scanSpecial() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance();
    
    switch (c) {
        case '@': return Token(TokenType::AT, "@", startLine, startColumn, 1);
        case '#': return Token(TokenType::HASH, "#", startLine, startColumn, 1);
        case '&': return Token(TokenType::AMPERSAND, "&", startLine, startColumn, 1);
        case '$': return Token(TokenType::DOLLAR, "$", startLine, startColumn, 1);
        case '?': return Token(TokenType::QUESTION, "?", startLine, startColumn, 1);
        case '!': return Token(TokenType::EXCLAMATION, "!", startLine, startColumn, 1);
        case '_': return Token(TokenType::UNDERSCORE, "_", startLine, startColumn, 1);
        default: return createErrorToken("未知特殊符号: " + std::string(1, c));
    }
}

Token CHTLLexer::scanText() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (!isAtEnd() && 
           !isWhitespace(currentChar()) && 
           currentChar() != ';' && 
           currentChar() != ':' && 
           currentChar() != '=' &&
           currentChar() != '{' &&
           currentChar() != '}' &&
           currentChar() != '[' &&
           currentChar() != ']' &&
           currentChar() != '(' &&
           currentChar() != ')' &&
           currentChar() != ',' &&
           currentChar() != '.' &&
           currentChar() != '@' &&
           currentChar() != '#' &&
           currentChar() != '&' &&
           currentChar() != '$' &&
           currentChar() != '?' &&
           currentChar() != '!' &&
           currentChar() != '_' &&
           currentChar() != '/' &&
           currentChar() != '\n' &&
           currentChar() != '\r') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return Token(TokenType::TEXT, value, startLine, startColumn, value.length());
}

Token CHTLLexer::scanWhitespace() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isWhitespace(currentChar()) && !isAtEnd()) {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return Token(TokenType::WHITESPACE, value, startLine, startColumn, value.length());
}

char CHTLLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return input_[currentPos_];
}

char CHTLLexer::peekChar(size_t offset) const {
    if (currentPos_ + offset >= input_.length()) return '\0';
    return input_[currentPos_ + offset];
}

void CHTLLexer::advance(size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (currentPos_ < input_.length()) {
            if (input_[currentPos_] == '\n') {
                currentLine_++;
                currentColumn_ = 1;
            } else {
                currentColumn_++;
            }
            currentPos_++;
        }
    }
}

bool CHTLLexer::isAtEnd() const {
    return currentPos_ >= input_.length();
}

bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n';
}

void CHTLLexer::skipWhitespace() {
    while (isWhitespace(currentChar()) && !isAtEnd()) {
        advance();
    }
}

Token CHTLLexer::createErrorToken(const std::string& message) {
    return Token(TokenType::ERROR, message, currentLine_, currentColumn_, 0);
}