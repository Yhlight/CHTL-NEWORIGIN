#include "CHTLLexer.h"
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <functional>

namespace CHTL {

// 静态成员初始化
std::unordered_map<std::string, TokenType> CHTLLexer::keywordMap_;
std::unordered_map<std::string, TokenType> CHTLLexer::templateMap_;
std::unordered_map<std::string, TokenType> CHTLLexer::operatorMap_;
bool CHTLLexer::mapsInitialized_ = false;

CHTLLexer::CHTLLexer() 
    : position_(0), line_(1), column_(1), 
      skipWhitespace_(true), skipComments_(false), caseSensitive_(false) {
    if (!mapsInitialized_) {
        initializeMaps();
        mapsInitialized_ = true;
    }
}

CHTLLexer::~CHTLLexer() = default;

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    setInput(input);
    std::vector<Token> tokens;
    
    while (position_ < input_.length()) {
        Token token = nextToken();
        if (!token.isEmpty()) {
            // 根据选项过滤token
            if (skipWhitespace_ && token.isWhitespace()) {
                continue;
            }
            if (skipComments_ && token.is(TokenType::COMMENT)) {
                continue;
            }
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line_, column_, position_);
    return tokens;
}

std::vector<Token> CHTLLexer::tokenizeFile(const std::string& filename) {
    // 这里需要实现文件读取，暂时返回空
    return {};
}

void CHTLLexer::setSkipWhitespace(bool skip) {
    skipWhitespace_ = skip;
}

void CHTLLexer::setSkipComments(bool skip) {
    skipComments_ = skip;
}

void CHTLLexer::setCaseSensitive(bool sensitive) {
    caseSensitive_ = sensitive;
}

bool CHTLLexer::isSkipWhitespace() const {
    return skipWhitespace_;
}

bool CHTLLexer::isSkipComments() const {
    return skipComments_;
}

bool CHTLLexer::isCaseSensitive() const {
    return caseSensitive_;
}

const std::vector<CHTLLexer::LexerError>& CHTLLexer::getErrors() const {
    return errors_;
}

bool CHTLLexer::hasErrors() const {
    return !errors_.empty();
}

void CHTLLexer::clearErrors() {
    errors_.clear();
}

Token CHTLLexer::nextToken() {
    if (position_ >= input_.length()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_, position_);
    }
    
    char c = currentChar();
    
    // 跳过空白字符
    if (skipWhitespace_ && isWhitespace(c)) {
        return readWhitespace();
    }
    
    // 处理换行符
    if (isNewline(c)) {
        return readNewline();
    }
    
    // 处理标识符
    if (isIdentifierStart(c)) {
        return readIdentifier();
    }
    
    // 处理数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 处理字符串
    if (isStringDelimiter(c)) {
        return readString();
    }
    
    // 处理注释
    if (isCommentStart(c)) {
        return readComment();
    }
    
    // 处理操作符
    if (isOperatorChar(c)) {
        return readOperator();
    }
    
    // 处理标点符号
    if (isPunctuationChar(c)) {
        return readPunctuation();
    }
    
    // 未知字符
    reportError("Unknown character: " + std::string(1, c));
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_, position_ - 1);
}

Token CHTLLexer::readIdentifier() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string identifier = readIdentifierName();
    TokenType type = classifyIdentifier(identifier);
    
    return Token(type, identifier, startLine, startColumn, startPos);
}

Token CHTLLexer::readString() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    char delimiter = currentChar();
    advance(); // 跳过开始引号
    
    std::string value = readStringLiteral(delimiter);
    
    if (currentChar() == delimiter) {
        advance(); // 跳过结束引号
    } else {
        reportError("Unterminated string literal");
    }
    
    return Token(TokenType::STRING, value, startLine, startColumn, startPos);
}

Token CHTLLexer::readNumber() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string number;
    
    // 检查十六进制
    if (currentChar() == '0' && peekChar() == 'x') {
        advance(); advance();
        number = "0x" + readHexNumber();
    }
    // 检查二进制
    else if (currentChar() == '0' && peekChar() == 'b') {
        advance(); advance();
        number = "0b" + readBinaryNumber();
    }
    // 检查八进制
    else if (currentChar() == '0' && peekChar() >= '0' && peekChar() <= '7') {
        number = readOctalNumber();
    }
    // 普通数字
    else {
        number = readInteger();
        if (currentChar() == '.' && isDigit(peekChar())) {
            advance();
            number += "." + readInteger();
        }
    }
    
    return Token(TokenType::NUMBER, number, startLine, startColumn, startPos);
}

Token CHTLLexer::readComment() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string comment;
    
    if (currentChar() == '/' && peekChar() == '/') {
        comment = readLineComment();
    } else if (currentChar() == '/' && peekChar() == '*') {
        comment = readBlockComment();
    }
    
    return Token(TokenType::COMMENT, comment, startLine, startColumn, startPos);
}

Token CHTLLexer::readOperator() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string op = readOperatorSequence();
    TokenType type = classifyOperator(op);
    
    return Token(type, op, startLine, startColumn, startPos);
}

Token CHTLLexer::readPunctuation() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    char c = currentChar();
    advance();
    
    TokenType type = TokenType::UNKNOWN;
    switch (c) {
        case '(': type = TokenType::LEFT_PAREN; break;
        case ')': type = TokenType::RIGHT_PAREN; break;
        case '[': type = TokenType::LEFT_BRACKET; break;
        case ']': type = TokenType::RIGHT_BRACKET; break;
        case '{': type = TokenType::LEFT_BRACE; break;
        case '}': type = TokenType::RIGHT_BRACE; break;
        case ';': type = TokenType::SEMICOLON; break;
        case ',': type = TokenType::COMMA; break;
        case '.': type = TokenType::DOT; break;
        case '?': type = TokenType::QUESTION; break;
        case '!': type = TokenType::EXCLAMATION; break;
        case '#': type = TokenType::HASH; break;
        case '@': type = TokenType::AT; break;
        case '$': type = TokenType::DOLLAR; break;
        case '&': type = TokenType::AMPERSAND; break;
        case '|': type = TokenType::PIPE; break;
        case '~': type = TokenType::TILDE; break;
        case '^': type = TokenType::CARET; break;
        default: type = TokenType::UNKNOWN; break;
    }
    
    return Token(type, std::string(1, c), startLine, startColumn, startPos);
}

Token CHTLLexer::readWhitespace() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    std::string whitespace;
    while (position_ < input_.length() && isWhitespace(currentChar())) {
        whitespace += currentChar();
        advance();
    }
    
    return Token(TokenType::WHITESPACE, whitespace, startLine, startColumn, startPos);
}

Token CHTLLexer::readNewline() {
    size_t startPos = position_;
    size_t startLine = line_;
    size_t startColumn = column_;
    
    char c = currentChar();
    advance();
    
    // 处理\r\n
    if (c == '\r' && currentChar() == '\n') {
        advance();
    }
    
    return Token(TokenType::NEWLINE, std::string(1, c), startLine, startColumn, startPos);
}

char CHTLLexer::currentChar() const {
    return (position_ < input_.length()) ? input_[position_] : '\0';
}

char CHTLLexer::peekChar() const {
    return (position_ + 1 < input_.length()) ? input_[position_ + 1] : '\0';
}

char CHTLLexer::nextChar() {
    if (position_ < input_.length()) {
        char c = input_[position_];
        advance();
        return c;
    }
    return '\0';
}

void CHTLLexer::advance() {
    if (position_ < input_.length()) {
        updatePosition(input_[position_]);
        position_++;
    }
}

void CHTLLexer::skipWhitespace() {
    while (position_ < input_.length() && isWhitespace(currentChar())) {
        advance();
    }
}

bool CHTLLexer::isIdentifierStart(char c) const {
    return TokenUtil::isIdentifierStartChar(c);
}

bool CHTLLexer::isIdentifierChar(char c) const {
    return TokenUtil::isIdentifierChar(c);
}

bool CHTLLexer::isDigit(char c) const {
    return TokenUtil::isDigit(c);
}

bool CHTLLexer::isHexDigit(char c) const {
    return TokenUtil::isHexDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return TokenUtil::isWhitespace(c);
}

bool CHTLLexer::isNewline(char c) const {
    return TokenUtil::isNewline(c);
}

bool CHTLLexer::isStringDelimiter(char c) const {
    return TokenUtil::isStringDelimiter(c);
}

bool CHTLLexer::isCommentStart(char c) const {
    return TokenUtil::isCommentStart(c, peekChar());
}

bool CHTLLexer::isOperatorChar(char c) const {
    return TokenUtil::isOperatorChar(c);
}

bool CHTLLexer::isPunctuationChar(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || 
           c == '{' || c == '}' || c == ';' || c == ',' || 
           c == '.' || c == '?' || c == '!' || c == '#' || 
           c == '@' || c == '$' || c == '&' || c == '|' || 
           c == '~' || c == '^';
}

std::string CHTLLexer::readStringLiteral(char delimiter) {
    std::string result;
    
    while (position_ < input_.length() && currentChar() != delimiter) {
        char c = currentChar();
        
        if (c == '\\') {
            advance();
            char next = currentChar();
            switch (next) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += next; break;
            }
            advance();
        } else {
            result += c;
            advance();
        }
    }
    
    return result;
}

std::string CHTLLexer::readRawString() {
    // 实现原始字符串读取
    return "";
}

std::string CHTLLexer::readTemplateString() {
    // 实现模板字符串读取
    return "";
}

std::string CHTLLexer::readInteger() {
    std::string result;
    while (position_ < input_.length() && isDigit(currentChar())) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readFloat() {
    std::string result = readInteger();
    if (currentChar() == '.') {
        result += currentChar();
        advance();
        result += readInteger();
    }
    return result;
}

std::string CHTLLexer::readHexNumber() {
    std::string result;
    while (position_ < input_.length() && isHexDigit(currentChar())) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readBinaryNumber() {
    std::string result;
    while (position_ < input_.length() && 
           (currentChar() == '0' || currentChar() == '1')) {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readOctalNumber() {
    std::string result;
    while (position_ < input_.length() && 
           currentChar() >= '0' && currentChar() <= '7') {
        result += currentChar();
        advance();
    }
    return result;
}

std::string CHTLLexer::readIdentifierName() {
    std::string result;
    
    // 读取第一个字符
    if (isIdentifierStart(currentChar())) {
        result += currentChar();
        advance();
    }
    
    // 读取后续字符
    while (position_ < input_.length() && isIdentifierChar(currentChar())) {
        result += currentChar();
        advance();
    }
    
    return result;
}

TokenType CHTLLexer::classifyIdentifier(const std::string& identifier) {
    std::string key = toCase(identifier);
    
    // 检查关键字
    auto it = keywordMap_.find(key);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    
    // 检查模板关键字
    it = templateMap_.find(key);
    if (it != templateMap_.end()) {
        return it->second;
    }
    
    return TokenType::IDENTIFIER;
}

std::string CHTLLexer::readOperatorSequence() {
    std::string result;
    
    while (position_ < input_.length() && isOperatorChar(currentChar())) {
        result += currentChar();
        advance();
    }
    
    return result;
}

TokenType CHTLLexer::classifyOperator(const std::string& op) {
    auto it = operatorMap_.find(op);
    if (it != operatorMap_.end()) {
        return it->second;
    }
    
    return TokenType::UNKNOWN;
}

std::string CHTLLexer::readLineComment() {
    std::string result;
    
    // 跳过 //
    advance(); advance();
    
    while (position_ < input_.length() && !isNewline(currentChar())) {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string CHTLLexer::readBlockComment() {
    std::string result;
    
    // 跳过 /*
    advance(); advance();
    
    while (position_ < input_.length()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(); advance();
            break;
        }
        result += currentChar();
        advance();
    }
    
    return result;
}

void CHTLLexer::reportError(const std::string& message) {
    reportError(message, line_, column_);
}

void CHTLLexer::reportError(const std::string& message, size_t line, size_t column) {
    LexerError error;
    error.line = line;
    error.column = column;
    error.message = message;
    error.context = getContext(line, column);
    errors_.push_back(error);
}

std::string CHTLLexer::getContext(size_t line, size_t column) const {
    // 简化实现，返回当前行内容
    return "";
}

void CHTLLexer::updatePosition(char c) {
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

void CHTLLexer::resetPosition() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
}

void CHTLLexer::reset() {
    resetPosition();
    errors_.clear();
}

void CHTLLexer::setInput(const std::string& input) {
    input_ = input;
    reset();
}

std::string CHTLLexer::toCase(const std::string& str) const {
    if (caseSensitive_) {
        return str;
    }
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool CHTLLexer::isKeyword(const std::string& str) const {
    return keywordMap_.find(toCase(str)) != keywordMap_.end();
}

bool CHTLLexer::isTemplateKeyword(const std::string& str) const {
    return templateMap_.find(toCase(str)) != templateMap_.end();
}

bool CHTLLexer::isOperatorKeyword(const std::string& str) const {
    return operatorMap_.find(str) != operatorMap_.end();
}

void CHTLLexer::initializeMaps() {
    // 初始化关键字映射
    keywordMap_ = {
        {"html", TokenType::KEYWORD_HTML},
        {"head", TokenType::KEYWORD_HEAD},
        {"body", TokenType::KEYWORD_BODY},
        {"div", TokenType::KEYWORD_DIV},
        {"span", TokenType::KEYWORD_SPAN},
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"script", TokenType::KEYWORD_SCRIPT},
        {"[template]", TokenType::KEYWORD_TEMPLATE},
        {"[custom]", TokenType::KEYWORD_CUSTOM},
        {"[origin]", TokenType::KEYWORD_ORIGIN},
        {"[import]", TokenType::KEYWORD_IMPORT},
        {"[namespace]", TokenType::KEYWORD_NAMESPACE},
        {"[configuration]", TokenType::KEYWORD_CONFIGURATION},
        {"[info]", TokenType::KEYWORD_INFO},
        {"[export]", TokenType::KEYWORD_EXPORT}
    };
    
    // 初始化模板映射
    templateMap_ = {
        {"@style", TokenType::TEMPLATE_STYLE},
        {"@element", TokenType::TEMPLATE_ELEMENT},
        {"@var", TokenType::TEMPLATE_VAR},
        {"@html", TokenType::TEMPLATE_HTML},
        {"@javascript", TokenType::TEMPLATE_JAVASCRIPT},
        {"@chtl", TokenType::TEMPLATE_CHTL},
        {"@cjmod", TokenType::TEMPLATE_CJMOD},
        {"@config", TokenType::TEMPLATE_CONFIG}
    };
    
    // 初始化操作符映射
    operatorMap_ = {
        {"=", TokenType::ASSIGN},
        {":", TokenType::COLON},
        {";", TokenType::SEMICOLON},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"?", TokenType::QUESTION},
        {"!", TokenType::EXCLAMATION},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULTIPLY},
        {"/", TokenType::DIVIDE},
        {"%", TokenType::MODULO},
        {"**", TokenType::POWER},
        {"==", TokenType::EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"<", TokenType::LESS},
        {"<=", TokenType::LESS_EQUAL},
        {">", TokenType::GREATER},
        {">=", TokenType::GREATER_EQUAL},
        {"&&", TokenType::AND},
        {"||", TokenType::OR}
    };
}

} // namespace CHTL