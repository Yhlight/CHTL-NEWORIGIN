#include "CHTLJSLexer.h"
#include <iostream>
#include <cctype>

CHTLJSLexer::CHTLJSLexer() 
    : currentPos_(0)
    , currentLine_(1)
    , currentColumn_(1)
    , debugMode_(false) {
    initializeKeywords();
}

bool CHTLJSLexer::tokenize(const std::string& input, std::vector<CHTLJSToken>& tokens) {
    input_ = input;
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    
    if (debugMode_) {
        std::cout << "开始CHTL JS词法分析，输入长度: " << input.length() << std::endl;
    }
    
    return scanTokens(tokens);
}

void CHTLJSLexer::initializeKeywords() {
    keywords_["Listen"] = CHTLJSTokenType::LISTEN;
    keywords_["Animate"] = CHTLJSTokenType::ANIMATE;
    keywords_["Router"] = CHTLJSTokenType::ROUTER;
    keywords_["Vir"] = CHTLJSTokenType::VIR;
    keywords_["iNeverAway"] = CHTLJSTokenType::INEVERAWAY;
    keywords_["printMylove"] = CHTLJSTokenType::PRINTMYLOVE;
    keywords_["ScriptLoader"] = CHTLJSTokenType::SCRIPTLOADER;
}

bool CHTLJSLexer::scanTokens(std::vector<CHTLJSToken>& tokens) {
    while (!isAtEnd()) {
        CHTLJSToken token = scanNextToken();
        tokens.push_back(token);
        
        if (token.type == CHTLJSTokenType::ERROR) {
            return false;
        }
        
        if (token.type == CHTLJSTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    if (debugMode_) {
        std::cout << "CHTL JS词法分析完成，共生成 " << tokens.size() << " 个token" << std::endl;
    }
    
    return true;
}

CHTLJSToken CHTLJSLexer::scanNextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", currentLine_, currentColumn_, 0);
    }
    
    char c = currentChar();
    
    // 检查选择器 {{...}}
    if (c == '{' && peekChar() == '{') {
        return scanSelector();
    }
    
    // 检查字符串
    if (c == '"' || c == '\'') {
        return scanString();
    }
    
    // 检查数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 检查布尔值
    if (c == 't' || c == 'f') {
        std::string potential = input_.substr(currentPos_, 4);
        if (potential == "true" || potential == "false") {
            return scanBoolean();
        }
    }
    
    // 检查标识符和关键字
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 检查运算符
    if (c == '-' && peekChar() == '>') {
        return scanOperator();
    }
    if (c == '&' && peekChar() == '-' && peekChar(2) == '>') {
        return scanOperator();
    }
    if (c == '.' || c == ',' || c == ':' || c == ';') {
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

CHTLJSToken CHTLJSLexer::scanIdentifier() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isAlphaNumeric(currentChar()) || currentChar() == '_') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    
    // 检查是否是关键字
    auto it = keywords_.find(value);
    if (it != keywords_.end()) {
        return CHTLJSToken(it->second, value, startLine, startColumn, value.length());
    }
    
    return CHTLJSToken(CHTLJSTokenType::IDENTIFIER, value, startLine, startColumn, value.length());
}

CHTLJSToken CHTLJSLexer::scanString() {
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
    return CHTLJSToken(CHTLJSTokenType::STRING, value, startLine, startColumn, value.length() + 2);
}

CHTLJSToken CHTLJSLexer::scanNumber() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isDigit(currentChar()) || currentChar() == '.') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return CHTLJSToken(CHTLJSTokenType::NUMBER, value, startLine, startColumn, value.length());
}

CHTLJSToken CHTLJSLexer::scanBoolean() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    if (input_.substr(currentPos_, 4) == "true") {
        advance(4);
        return CHTLJSToken(CHTLJSTokenType::BOOLEAN, "true", startLine, startColumn, 4);
    } else if (input_.substr(currentPos_, 5) == "false") {
        advance(5);
        return CHTLJSToken(CHTLJSTokenType::BOOLEAN, "false", startLine, startColumn, 5);
    }
    
    return createErrorToken("无效的布尔值");
}

CHTLJSToken CHTLJSLexer::scanSelector() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance(2); // 跳过 {{
    
    while (!isAtEnd() && !(currentChar() == '}' && peekChar() == '}')) {
        advance();
    }
    
    if (isAtEnd()) {
        return createErrorToken("未闭合的选择器");
    }
    
    advance(2); // 跳过 }}
    
    std::string value = input_.substr(start, currentPos_ - start);
    return CHTLJSToken(CHTLJSTokenType::SELECTOR, value, startLine, startColumn, value.length());
}

CHTLJSToken CHTLJSLexer::scanOperator() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    if (c == '-' && peekChar() == '>') {
        advance(2);
        return CHTLJSToken(CHTLJSTokenType::ARROW, "->", startLine, startColumn, 2);
    } else if (c == '&' && peekChar() == '-' && peekChar(2) == '>') {
        advance(3);
        return CHTLJSToken(CHTLJSTokenType::AMPERSAND_ARROW, "&->", startLine, startColumn, 3);
    } else {
        advance();
        switch (c) {
            case '.': return CHTLJSToken(CHTLJSTokenType::DOT, ".", startLine, startColumn, 1);
            case ',': return CHTLJSToken(CHTLJSTokenType::COMMA, ",", startLine, startColumn, 1);
            case ':': return CHTLJSToken(CHTLJSTokenType::COLON, ":", startLine, startColumn, 1);
            case ';': return CHTLJSToken(CHTLJSTokenType::SEMICOLON, ";", startLine, startColumn, 1);
            default: return createErrorToken("未知运算符: " + std::string(1, c));
        }
    }
}

CHTLJSToken CHTLJSLexer::scanDelimiter() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance();
    
    switch (c) {
        case '{': return CHTLJSToken(CHTLJSTokenType::LEFT_BRACE, "{", startLine, startColumn, 1);
        case '}': return CHTLJSToken(CHTLJSTokenType::RIGHT_BRACE, "}", startLine, startColumn, 1);
        case '[': return CHTLJSToken(CHTLJSTokenType::LEFT_BRACKET, "[", startLine, startColumn, 1);
        case ']': return CHTLJSToken(CHTLJSTokenType::RIGHT_BRACKET, "]", startLine, startColumn, 1);
        case '(': return CHTLJSToken(CHTLJSTokenType::LEFT_PAREN, "(", startLine, startColumn, 1);
        case ')': return CHTLJSToken(CHTLJSTokenType::RIGHT_PAREN, ")", startLine, startColumn, 1);
        default: return createErrorToken("未知分隔符: " + std::string(1, c));
    }
}

CHTLJSToken CHTLJSLexer::scanSpecial() {
    char c = currentChar();
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    advance();
    
    switch (c) {
        case '@': return CHTLJSToken(CHTLJSTokenType::AT, "@", startLine, startColumn, 1);
        case '#': return CHTLJSToken(CHTLJSTokenType::HASH, "#", startLine, startColumn, 1);
        case '&': return CHTLJSToken(CHTLJSTokenType::AMPERSAND, "&", startLine, startColumn, 1);
        case '$': return CHTLJSToken(CHTLJSTokenType::DOLLAR, "$", startLine, startColumn, 1);
        case '?': return CHTLJSToken(CHTLJSTokenType::QUESTION, "?", startLine, startColumn, 1);
        case '!': return CHTLJSToken(CHTLJSTokenType::EXCLAMATION, "!", startLine, startColumn, 1);
        case '_': return CHTLJSToken(CHTLJSTokenType::UNDERSCORE, "_", startLine, startColumn, 1);
        default: return createErrorToken("未知特殊符号: " + std::string(1, c));
    }
}

CHTLJSToken CHTLJSLexer::scanText() {
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
           currentChar() != '-' &&
           currentChar() != '\n' &&
           currentChar() != '\r') {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return CHTLJSToken(CHTLJSTokenType::TEXT, value, startLine, startColumn, value.length());
}

CHTLJSToken CHTLJSLexer::scanWhitespace() {
    size_t start = currentPos_;
    size_t startLine = currentLine_;
    size_t startColumn = currentColumn_;
    
    while (isWhitespace(currentChar()) && !isAtEnd()) {
        advance();
    }
    
    std::string value = input_.substr(start, currentPos_ - start);
    return CHTLJSToken(CHTLJSTokenType::WHITESPACE, value, startLine, startColumn, value.length());
}

char CHTLJSLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return input_[currentPos_];
}

char CHTLJSLexer::peekChar(size_t offset) const {
    if (currentPos_ + offset >= input_.length()) return '\0';
    return input_[currentPos_ + offset];
}

void CHTLJSLexer::advance(size_t count) {
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

bool CHTLJSLexer::isAtEnd() const {
    return currentPos_ >= input_.length();
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n';
}

void CHTLJSLexer::skipWhitespace() {
    while (isWhitespace(currentChar()) && !isAtEnd()) {
        advance();
    }
}

CHTLJSToken CHTLJSLexer::createErrorToken(const std::string& message) {
    return CHTLJSToken(CHTLJSTokenType::ERROR, message, currentLine_, currentColumn_, 0);
}