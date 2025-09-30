#include "CHTL/CHTLLexer.h"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <fstream>

namespace CHTL {

// Token implementation
std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token{type=" << static_cast<int>(type) 
       << ", value='" << value 
       << "', line=" << line 
       << ", column=" << column << "}";
    return ss.str();
}

// GlobalMap implementation
GlobalMap::GlobalMap() = default;

void GlobalMap::addIdentifier(const std::string& identifier, const std::string& value) {
    identifiers_[identifier] = value;
}

std::string GlobalMap::getIdentifier(const std::string& identifier) const {
    auto it = identifiers_.find(identifier);
    if (it != identifiers_.end()) {
        return it->second;
    }
    return "";
}

bool GlobalMap::hasIdentifier(const std::string& identifier) const {
    return identifiers_.find(identifier) != identifiers_.end();
}

void GlobalMap::removeIdentifier(const std::string& identifier) {
    identifiers_.erase(identifier);
}

void GlobalMap::clear() {
    identifiers_.clear();
}

const std::unordered_map<std::string, std::string>& GlobalMap::getAllIdentifiers() const {
    return identifiers_;
}

// CHTLLexer implementation
CHTLLexer::CHTLLexer() 
    : position_(0)
    , line_(1)
    , column_(1)
    , currentTokenIndex_(0) {
}

void CHTLLexer::initialize() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    currentTokenIndex_ = 0;
    tokens_.clear();
    errorMessage_.clear();
    globalMap_.clear();
}

std::vector<Token> CHTLLexer::tokenize(const std::string& input) {
    input_ = input;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    currentTokenIndex_ = 0;
    tokens_.clear();
    errorMessage_.clear();
    
    tokenizeInput();
    return tokens_;
}

std::vector<Token> CHTLLexer::tokenizeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        errorMessage_ = "Cannot open file: " + filePath;
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return tokenize(buffer.str());
}

const Token& CHTLLexer::getCurrentToken() const {
    return currentToken_;
}

bool CHTLLexer::advance() {
    if (currentTokenIndex_ >= tokens_.size()) {
        return false;
    }
    currentToken_ = tokens_[currentTokenIndex_++];
    return true;
}

Token CHTLLexer::peek() const {
    if (currentTokenIndex_ >= tokens_.size()) {
        return Token(TokenType::EOF_TOKEN);
    }
    return tokens_[currentTokenIndex_];
}

bool CHTLLexer::isAtEnd() const {
    return currentTokenIndex_ >= tokens_.size();
}

void CHTLLexer::reset() {
    currentTokenIndex_ = 0;
    currentToken_ = Token();
}

GlobalMap& CHTLLexer::getGlobalMap() {
    return globalMap_;
}

std::string CHTLLexer::getErrorMessage() const {
    return errorMessage_;
}

const std::vector<Token>& CHTLLexer::getTokens() const {
    return tokens_;
}

void CHTLLexer::tokenizeInput() {
    while (position_ < input_.length()) {
        skipWhitespace();
        
        if (position_ >= input_.length()) {
            break;
        }
        
        char c = input_[position_];
        
        if (c == '/' && peekChar() == '/') {
            skipComment();
        } else if (c == '/' && peekChar() == '*') {
            skipMultilineComment();
        } else if (c == '#' && isWhitespace(peekChar())) {
            addToken(TokenType::GENERATOR_COMMENT);
            advanceChar();
        } else if (c == '"' || c == '\'') {
            Token token = parseStringLiteral();
            addToken(token.type, token.value);
        } else if (isDigit(c)) {
            Token token = parseNumberLiteral();
            addToken(token.type, token.value);
        } else if (isAlpha(c) || c == '_') {
            Token token = parseIdentifier();
            addToken(token.type, token.value);
        } else if (isOperator(c)) {
            Token token = parseOperator();
            addToken(token.type, token.value);
        } else if (isSpecial(c)) {
            Token token = parseSpecial();
            addToken(token.type, token.value);
        } else {
            advanceChar();
        }
    }
    
    addToken(TokenType::EOF_TOKEN);
}

Token CHTLLexer::getNextToken() {
    if (currentTokenIndex_ >= tokens_.size()) {
        return Token(TokenType::EOF_TOKEN);
    }
    return tokens_[currentTokenIndex_++];
}

void CHTLLexer::skipWhitespace() {
    while (position_ < input_.length() && isWhitespace(input_[position_])) {
        if (input_[position_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLLexer::skipComment() {
    while (position_ < input_.length() && input_[position_] != '\n') {
        position_++;
        column_++;
    }
    if (position_ < input_.length()) {
        position_++;
        line_++;
        column_ = 1;
    }
}

void CHTLLexer::skipMultilineComment() {
    position_ += 2; // Skip /*
    column_ += 2;
    
    while (position_ < input_.length() - 1) {
        if (input_[position_] == '*' && input_[position_ + 1] == '/') {
            position_ += 2;
            column_ += 2;
            return;
        }
        if (input_[position_] == '\n') {
            line_++;
            column_ = 1;
            position_++;
        } else {
            column_++;
            position_++;
        }
    }
    
    // If we reach here, the comment wasn't properly closed
    errorMessage_ = "Unterminated multiline comment";
}

Token CHTLLexer::parseStringLiteral() {
    char quote = input_[position_];
    advanceChar(); // Skip opening quote
    
    std::string value;
    while (position_ < input_.length() && input_[position_] != quote) {
        if (input_[position_] == '\\' && position_ + 1 < input_.length()) {
            advanceChar(); // Skip backslash
            char escaped = advanceChar();
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
            value += advanceChar();
        }
    }
    
    if (position_ < input_.length()) {
        advanceChar(); // Skip closing quote
    }
    
    return Token(TokenType::STRING_LITERAL, value, line_, column_);
}

Token CHTLLexer::parseNumberLiteral() {
    std::string value;
    
    // Check for hexadecimal number
    if (position_ + 2 < input_.length() && 
        input_[position_] == '0' && 
        (input_[position_ + 1] == 'x' || input_[position_ + 1] == 'X')) {
        // Parse hexadecimal number
        value += advanceChar(); // '0'
        value += advanceChar(); // 'x' or 'X'
        
        while (position_ < input_.length() && 
               (isDigit(input_[position_]) || 
                (input_[position_] >= 'a' && input_[position_] <= 'f') ||
                (input_[position_] >= 'A' && input_[position_] <= 'F'))) {
            value += advanceChar();
        }
        
        return Token(TokenType::NUMBER_LITERAL, value, line_, column_);
    }
    
    // Parse decimal number
    while (position_ < input_.length() && (isDigit(input_[position_]) || input_[position_] == '.')) {
        value += advanceChar();
    }
    
    return Token(TokenType::NUMBER_LITERAL, value, line_, column_);
}

Token CHTLLexer::parseIdentifier() {
    std::string value;
    
    while (position_ < input_.length() && isAlphaNumeric(input_[position_])) {
        value += advanceChar();
    }
    
    // Check if it's a keyword
    TokenType type = getKeywordType(value);
    if (type != TokenType::IDENTIFIER) {
        return Token(type, value, line_, column_);
    }
    
    return Token(TokenType::IDENTIFIER, value, line_, column_);
}

Token CHTLLexer::parseOperator() {
    char c = advanceChar();
    
    switch (c) {
        case ':': return Token(TokenType::COLON, ":", line_, column_);
        case '=': 
            if (peekChar() == '=') {
                advanceChar();
                return Token(TokenType::EQUAL, "==", line_, column_);
            }
            return Token(TokenType::EQUALS, "=", line_, column_);
        case ';': return Token(TokenType::SEMICOLON, ";", line_, column_);
        case ',': return Token(TokenType::COMMA, ",", line_, column_);
        case '.': return Token(TokenType::DOT, ".", line_, column_);
        case '#': return Token(TokenType::HASH, "#", line_, column_);
        case '@': return Token(TokenType::AT, "@", line_, column_);
        case '?': return Token(TokenType::QUESTION, "?", line_, column_);
        case '!': 
            if (peekChar() == '=') {
                advanceChar();
                return Token(TokenType::NOT_EQUAL, "!=", line_, column_);
            }
            return Token(TokenType::EXCLAMATION, "!", line_, column_);
        case '+': return Token(TokenType::PLUS, "+", line_, column_);
        case '-': return Token(TokenType::MINUS, "-", line_, column_);
        case '*': 
            if (peekChar() == '*') {
                advanceChar();
                return Token(TokenType::POWER, "**", line_, column_);
            }
            return Token(TokenType::MULTIPLY, "*", line_, column_);
        case '/': return Token(TokenType::DIVIDE, "/", line_, column_);
        case '%': return Token(TokenType::MODULO, "%", line_, column_);
        case '<': 
            if (peekChar() == '=') {
                advanceChar();
                return Token(TokenType::LESS_EQUAL, "<=", line_, column_);
            }
            return Token(TokenType::LESS_THAN, "<", line_, column_);
        case '>': 
            if (peekChar() == '=') {
                advanceChar();
                return Token(TokenType::GREATER_EQUAL, ">=", line_, column_);
            }
            return Token(TokenType::GREATER_THAN, ">", line_, column_);
        case '&': 
            if (peekChar() == '&') {
                advanceChar();
                return Token(TokenType::AND, "&&", line_, column_);
            }
            break;
        case '|': 
            if (peekChar() == '|') {
                advanceChar();
                return Token(TokenType::OR, "||", line_, column_);
            }
            break;
    }
    
    return Token(TokenType::ERROR, std::string(1, c), line_, column_);
}

Token CHTLLexer::parseSpecial() {
    char c = advanceChar();
    
    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", line_, column_);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", line_, column_);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", line_, column_);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", line_, column_);
        case '{': return Token(TokenType::LEFT_BRACE, "{", line_, column_);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", line_, column_);
        case '<': return Token(TokenType::LEFT_ANGLE, "<", line_, column_);
        case '>': return Token(TokenType::RIGHT_ANGLE, ">", line_, column_);
    }
    
    return Token(TokenType::ERROR, std::string(1, c), line_, column_);
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
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool CHTLLexer::isOperator(char c) const {
    return c == ':' || c == '=' || c == ';' || c == ',' || c == '.' || 
           c == '#' || c == '@' || c == '?' || c == '!' ||
           c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '<' || c == '>' || c == '&' || c == '|';
}

bool CHTLLexer::isSpecial(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || 
           c == '{' || c == '}' || c == '<' || c == '>';
}

char CHTLLexer::peekChar() const {
    if (position_ >= input_.length()) {
        return '\0';
    }
    return input_[position_];
}

char CHTLLexer::peekChar(size_t offset) const {
    if (position_ + offset >= input_.length()) {
        return '\0';
    }
    return input_[position_ + offset];
}

char CHTLLexer::advanceChar() {
    if (position_ >= input_.length()) {
        return '\0';
    }
    char c = input_[position_++];
    column_++;
    return c;
}

bool CHTLLexer::matchChar(char expected) {
    if (peekChar() == expected) {
        advanceChar();
        return true;
    }
    return false;
}

void CHTLLexer::addToken(TokenType type, const std::string& value) {
    tokens_.emplace_back(type, value, line_, column_);
}

TokenType CHTLLexer::getKeywordType(const std::string& keyword) const {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"template", TokenType::TEMPLATE},
        {"custom", TokenType::CUSTOM},
        {"origin", TokenType::ORIGIN},
        {"import", TokenType::IMPORT},
        {"config", TokenType::CONFIG},
        {"namespace", TokenType::NAMESPACE},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP},
        {"top", TokenType::AT_TOP},
        {"bottom", TokenType::AT_BOTTOM},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"use", TokenType::USE},
        {"html5", TokenType::HTML5}
    };
    
    auto it = keywords.find(keyword);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

bool CHTLLexer::isKeyword(const std::string& word) const {
    return getKeywordType(word) != TokenType::IDENTIFIER;
}

} // namespace CHTL