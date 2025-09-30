#include "CHTL/CHTLLexer/Lexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace CHTL {

Lexer::Lexer() 
    : position_(0), line_(1), column_(1), hasError_(false), 
      caseSensitive_(false), preserveWhitespace_(true), preserveComments_(true) {
    initializeKeywords();
}

void Lexer::initializeKeywords() {
    // CHTL keywords
    keywords_["text"] = TokenType::TEXT_KEYWORD;
    keywords_["style"] = TokenType::STYLE_KEYWORD;
    keywords_["script"] = TokenType::SCRIPT_KEYWORD;
    keywords_["use"] = TokenType::USE_KEYWORD;
    keywords_["inherit"] = TokenType::INHERIT_KEYWORD;
    keywords_["delete"] = TokenType::DELETE_KEYWORD;
    keywords_["insert"] = TokenType::INSERT_KEYWORD;
    keywords_["except"] = TokenType::EXCEPT_KEYWORD;
    keywords_["from"] = TokenType::FROM_KEYWORD;
    keywords_["as"] = TokenType::AS_KEYWORD;
    keywords_["@"] = TokenType::AT_KEYWORD;
    
    // CHTL JS keywords
    keywords_["Vir"] = TokenType::VIR_KEYWORD;
    keywords_["Listen"] = TokenType::LISTEN_KEYWORD;
    keywords_["Animate"] = TokenType::ANIMATE_KEYWORD;
    keywords_["Router"] = TokenType::ROUTER_KEYWORD;
    keywords_["Delegate"] = TokenType::DELEGATE_KEYWORD;
    keywords_["ScriptLoader"] = TokenType::SCRIPT_LOADER_KEYWORD;
    
    // Boolean literals
    keywords_["true"] = TokenType::BOOLEAN_LITERAL;
    keywords_["false"] = TokenType::BOOLEAN_LITERAL;
    
    // CSS units (will be identified during parsing, not lexing)
    // keywords_["px"] = TokenType::CSS_UNIT;
    // keywords_["em"] = TokenType::CSS_UNIT;
    // keywords_["rem"] = TokenType::CSS_UNIT;
    // keywords_["%"] = TokenType::CSS_UNIT;
    // keywords_["vh"] = TokenType::CSS_UNIT;
    // keywords_["vw"] = TokenType::CSS_UNIT;
    // keywords_["vmin"] = TokenType::CSS_UNIT;
    // keywords_["vmax"] = TokenType::CSS_UNIT;
    // keywords_["pt"] = TokenType::CSS_UNIT;
    // keywords_["pc"] = TokenType::CSS_UNIT;
    // keywords_["in"] = TokenType::CSS_UNIT;
    // keywords_["cm"] = TokenType::CSS_UNIT;
    // keywords_["mm"] = TokenType::CSS_UNIT;
}

std::vector<Token> Lexer::tokenize(const std::string& source) {
    reset();
    source_ = source;
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        char c = current();
        
        if (isWhitespace(c)) {
            tokens.push_back(scanWhitespace());
            continue;
        }
        
        if (isAtEnd()) break;
        
        if (isAlpha(c) || c == '_') {
            tokens.push_back(scanIdentifier());
        } else if (isDigit(c)) {
            tokens.push_back(scanNumber());
        } else if (isQuote(c)) {
            tokens.push_back(scanString(c));
        } else if (c == '/' && peek() == '/') {
            tokens.push_back(scanComment());
        } else if (c == '/' && peek() == '*') {
            tokens.push_back(scanComment());
        } else if (c == '#') {
            // Check if it's a generator comment (must have space after #)
            if (peek() == ' ') {
                tokens.push_back(scanGeneratorComment());
            } else {
                tokens.push_back(createToken(TokenType::HASH, "#"));
            }
        } else if (c == '[') {
            tokens.push_back(scanTemplateKeyword());
        } else if (isOperator(c)) {
            tokens.push_back(scanOperator());
        } else if (isBracket(c)) {
            tokens.push_back(scanBracket());
        } else if (isWhitespace(c)) {
            tokens.push_back(scanWhitespace());
        } else if (isNewline(c)) {
            tokens.push_back(scanNewline());
        } else {
            setError("Unexpected character: " + std::string(1, c));
            break;
        }
    }
    
    if (!hasError_) {
        tokens.push_back(createToken(TokenType::EOF_TOKEN, ""));
    }
    
    return tokens;
}

std::vector<Token> Lexer::tokenizeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        setError("Could not open file: " + filePath);
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return tokenize(buffer.str());
}

void Lexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    hasError_ = false;
    errorMessage_.clear();
    errorLine_ = 0;
    errorColumn_ = 0;
}

void Lexer::setError(const std::string& message) {
    hasError_ = true;
    errorMessage_ = message;
    errorLine_ = line_;
    errorColumn_ = column_;
}

char Lexer::current() const {
    if (isAtEnd()) return '\0';
    return source_[position_];
}

char Lexer::peek(size_t offset) const {
    if (position_ + offset >= source_.length()) return '\0';
    return source_[position_ + offset];
}

bool Lexer::advance() {
    if (isAtEnd()) return false;
    
    if (current() == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    position_++;
    return true;
}

bool Lexer::isAtEnd() const {
    return position_ >= source_.length();
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t';
}

bool Lexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

bool Lexer::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool Lexer::isOperator(char c) const {
    return c == ':' || c == '=' || c == ';' || c == ',' || c == '.' || 
           c == '#' || c == '&' || c == '?' || c == '!' || c == '*' || 
           c == '+' || c == '-' || c == '/' || c == '%' || c == '^' || 
           c == '|' || c == '~' || c == '<' || c == '>';
}

bool Lexer::isBracket(char c) const {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

Token Lexer::createToken(TokenType type, const std::string& value) {
    return Token(type, value, line_, column_);
}

Token Lexer::createToken(TokenType type, char value) {
    return Token(type, std::string(1, value), line_, column_);
}

Token Lexer::scanIdentifier() {
    size_t start = position_;
    
    while (isAlphaNumeric(current()) || current() == '-') {
        advance();
    }
    
    std::string value = extractString(start, position_);
    
    // Check for special CHTL keywords
    if (value == "[Template]") {
        return createToken(TokenType::TEMPLATE_KEYWORD, value);
    } else if (value == "[Custom]") {
        return createToken(TokenType::CUSTOM_KEYWORD, value);
    } else if (value == "[Import]") {
        return createToken(TokenType::IMPORT_KEYWORD, value);
    } else if (value == "[Namespace]") {
        return createToken(TokenType::NAMESPACE_KEYWORD, value);
    } else if (value == "[Configuration]") {
        return createToken(TokenType::CONFIG_KEYWORD, value);
    } else if (value == "[Origin]") {
        return createToken(TokenType::ORIGIN_KEYWORD, value);
    }
    
    // Check if it's a keyword
    TokenType keywordType = getKeywordType(value);
    if (keywordType != TokenType::IDENTIFIER) {
        return createToken(keywordType, value);
    }
    
    return createToken(TokenType::IDENTIFIER, value);
}

Token Lexer::scanNumber() {
    size_t start = position_;
    
    while (isNumberChar(current())) {
        advance();
    }
    
    std::string value = extractString(start, position_);
    return createToken(TokenType::NUMBER_LITERAL, value);
}

Token Lexer::scanString(char quote) {
    advance(); // Skip opening quote
    size_t start = position_;
    
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\' && peek() == quote) {
            advance(); // Skip escape character
        }
        advance();
    }
    
    if (isAtEnd()) {
        setError("Unterminated string literal");
        return createToken(TokenType::ERROR_TOKEN, "");
    }
    
    std::string value = extractString(start, position_);
    advance(); // Skip closing quote
    
    TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_QUOTE_LITERAL;
    return createToken(type, value);
}

Token Lexer::scanComment() {
    size_t start = position_;
    
    if (peek() == '/') {
        // Line comment
        advance(); // Skip first '/'
        advance(); // Skip second '/'
        
        while (!isAtEnd() && !isNewline(current())) {
            advance();
        }
        
        std::string value = extractString(start, position_);
        return createToken(TokenType::LINE_COMMENT, value);
    } else if (peek() == '*') {
        // Block comment
        advance(); // Skip '/'
        advance(); // Skip '*'
        
        while (!isAtEnd() && !(current() == '*' && peek() == '/')) {
            advance();
        }
        
        if (isAtEnd()) {
            setError("Unterminated block comment");
            return createToken(TokenType::ERROR_TOKEN, "");
        }
        
        advance(); // Skip '*'
        advance(); // Skip '/'
        
        std::string value = extractString(start, position_);
        return createToken(TokenType::BLOCK_COMMENT, value);
    }
    
    return createToken(TokenType::SLASH, "/");
}

Token Lexer::scanGeneratorComment() {
    size_t start = position_;
    advance(); // Skip '#'
    
    // Generator comment must have space after #
    if (current() != ' ') {
        return createToken(TokenType::HASH, "#");
    }
    
    advance(); // Skip space
    
    while (!isAtEnd() && !isNewline(current())) {
        advance();
    }
    
    std::string value = extractString(start, position_);
    return createToken(TokenType::GENERATOR_COMMENT, value);
}

Token Lexer::scanTemplateKeyword() {
    size_t start = position_;
    
    while (!isAtEnd() && current() != ']') {
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // Skip ']'
    }
    
    std::string value = extractString(start, position_);
    
    if (value == "[Template]") {
        return createToken(TokenType::TEMPLATE_KEYWORD, value);
    } else if (value == "[Custom]") {
        return createToken(TokenType::CUSTOM_KEYWORD, value);
    } else if (value == "[Import]") {
        return createToken(TokenType::IMPORT_KEYWORD, value);
    } else if (value == "[Namespace]") {
        return createToken(TokenType::NAMESPACE_KEYWORD, value);
    } else if (value == "[Configuration]") {
        return createToken(TokenType::CONFIG_KEYWORD, value);
    } else if (value == "[Origin]") {
        return createToken(TokenType::ORIGIN_KEYWORD, value);
    }
    
    return createToken(TokenType::LEFT_BRACKET, "[");
}

Token Lexer::scanOperator() {
    char first = current();
    advance();
    
    if (isAtEnd()) {
        return createToken(recognizeOperator(std::string(1, first)), std::string(1, first));
    }
    
    char second = current();
    std::string twoChar = std::string(1, first) + std::string(1, second);
    
    // Special case for **= operator
    if (first == '*' && second == '*' && peek() == '=') {
        advance(); // Skip second *
        advance(); // Skip =
        return createToken(TokenType::POWER_EQUAL, "**=");
    }
    
    if (isMultiCharOperator(first, second)) {
        advance();
        return createToken(recognizeOperator(twoChar), twoChar);
    }
    
    return createToken(recognizeOperator(std::string(1, first)), std::string(1, first));
}

Token Lexer::scanBracket() {
    char c = current();
    advance();
    
    switch (c) {
        case '(': return createToken(TokenType::LEFT_PAREN, "(");
        case ')': return createToken(TokenType::RIGHT_PAREN, ")");
        case '[': return createToken(TokenType::LEFT_BRACKET, "[");
        case ']': return createToken(TokenType::RIGHT_BRACKET, "]");
        case '{': return createToken(TokenType::LEFT_BRACE, "{");
        case '}': return createToken(TokenType::RIGHT_BRACE, "}");
        case '<': return createToken(TokenType::LEFT_ANGLE, "<");
        case '>': return createToken(TokenType::RIGHT_ANGLE, ">");
        default: return createToken(TokenType::ERROR_TOKEN, "");
    }
}

Token Lexer::scanWhitespace() {
    size_t start = position_;
    
    while (isWhitespace(current())) {
        advance();
    }
    
    std::string value = extractString(start, position_);
    return createToken(TokenType::WHITESPACE, value);
}

Token Lexer::scanNewline() {
    char c = current();
    advance();
    
    if (c == '\r' && current() == '\n') {
        advance();
        return createToken(TokenType::NEWLINE, "\r\n");
    }
    
    return createToken(TokenType::NEWLINE, std::string(1, c));
}

void Lexer::skipWhitespace() {
    while (isWhitespace(current())) {
        advance();
    }
}

std::string Lexer::extractString(size_t start, size_t end) {
    return source_.substr(start, end - start);
}

bool Lexer::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

TokenType Lexer::getKeywordType(const std::string& word) const {
    auto it = keywords_.find(word);
    return it != keywords_.end() ? it->second : TokenType::IDENTIFIER;
}

TokenType Lexer::recognizeOperator(const std::string& op) const {
    static const std::unordered_map<std::string, TokenType> operators = {
        {":", TokenType::COLON},
        {"=", TokenType::EQUALS},
        {";", TokenType::SEMICOLON},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"#", TokenType::HASH},
        {"&", TokenType::AMPERSAND},
        {"?", TokenType::QUESTION},
        {"!", TokenType::EXCLAMATION},
        {"*", TokenType::ASTERISK},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"/", TokenType::SLASH},
        {"%", TokenType::PERCENT},
        {"^", TokenType::CARET},
        {"|", TokenType::PIPE},
        {"~", TokenType::TILDE},
        {"==", TokenType::EQUAL_EQUAL},
        {"!=", TokenType::NOT_EQUAL},
        {"<=", TokenType::LESS_EQUAL},
        {">=", TokenType::GREATER_EQUAL},
        {"<", TokenType::LESS_THAN},
        {">", TokenType::GREATER_THAN},
        {"&&", TokenType::AND_AND},
        {"||", TokenType::OR_OR},
        {"!", TokenType::NOT},
        {"++", TokenType::PLUS_PLUS},
        {"--", TokenType::MINUS_MINUS},
        {"+=", TokenType::PLUS_EQUAL},
        {"-=", TokenType::MINUS_EQUAL},
        {"*=", TokenType::MULTIPLY_EQUAL},
        {"/=", TokenType::DIVIDE_EQUAL},
        {"%=", TokenType::MODULO_EQUAL},
        {"**=", TokenType::POWER_EQUAL}
    };
    
    auto it = operators.find(op);
    return it != operators.end() ? it->second : TokenType::ERROR_TOKEN;
}

bool Lexer::isMultiCharOperator(char first, char second) const {
    return (first == '=' && second == '=') ||
           (first == '!' && second == '=') ||
           (first == '<' && second == '=') ||
           (first == '>' && second == '=') ||
           (first == '&' && second == '&') ||
           (first == '|' && second == '|') ||
           (first == '+' && second == '+') ||
           (first == '-' && second == '-') ||
           (first == '+' && second == '=') ||
           (first == '-' && second == '=') ||
           (first == '*' && second == '=') ||
           (first == '/' && second == '=') ||
           (first == '%' && second == '=') ||
           (first == '*' && second == '*');
}

bool Lexer::isNumberStart(char c) const {
    return isDigit(c) || c == '.';
}

bool Lexer::isNumberChar(char c) const {
    return isDigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-';
}

} // namespace CHTL