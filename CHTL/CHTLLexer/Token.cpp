#include "Token.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace CHTL {

bool Token::isOneOf(const std::vector<TokenType>& types) const {
    return std::find(types.begin(), types.end(), type) != types.end();
}

bool Token::isKeyword() const {
    return type >= TokenType::KEYWORD_HTML && type <= TokenType::KEYWORD_EXPORT;
}

bool Token::isOperator() const {
    return (type >= TokenType::ASSIGN && type <= TokenType::NOT) ||
           (type >= TokenType::PLUS && type <= TokenType::POWER) ||
           (type >= TokenType::EQUAL && type <= TokenType::GREATER_EQUAL) ||
           (type >= TokenType::AND && type <= TokenType::NOT);
}

bool Token::isBracket() const {
    return type >= TokenType::LEFT_PAREN && type <= TokenType::RIGHT_BRACE;
}

bool Token::isArithmeticOperator() const {
    return type >= TokenType::PLUS && type <= TokenType::POWER;
}

bool Token::isComparisonOperator() const {
    return type >= TokenType::EQUAL && type <= TokenType::GREATER_EQUAL;
}

bool Token::isLogicalOperator() const {
    return type >= TokenType::AND && type <= TokenType::NOT;
}

int Token::getPrecedence() const {
    return TokenUtil::getOperatorPrecedence(type);
}

bool Token::isLeftAssociative() const {
    return TokenUtil::isLeftAssociative(type);
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << TokenUtil::tokenTypeToString(type) 
        << ", \"" << value << "\", " << line << ":" << column << ")";
    return oss.str();
}

bool Token::isEmpty() const {
    return type == TokenType::UNKNOWN && value.empty();
}

bool Token::isWhitespace() const {
    return type == TokenType::WHITESPACE;
}

bool Token::isNewline() const {
    return type == TokenType::NEWLINE;
}

bool Token::isEOF() const {
    return type == TokenType::EOF_TOKEN;
}

// TokenStream实现
TokenStream::TokenStream(const std::vector<Token>& tokens) 
    : tokens_(tokens), currentPos_(0) {}

const Token& TokenStream::current() const {
    if (currentPos_ >= tokens_.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", 0, 0, 0);
        return eofToken;
    }
    return tokens_[currentPos_];
}

const Token& TokenStream::next() {
    if (currentPos_ < tokens_.size()) {
        currentPos_++;
    }
    return current();
}

const Token& TokenStream::peek() const {
    if (currentPos_ + 1 >= tokens_.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", 0, 0, 0);
        return eofToken;
    }
    return tokens_[currentPos_ + 1];
}

const Token& TokenStream::previous() const {
    if (currentPos_ == 0) {
        static Token emptyToken;
        return emptyToken;
    }
    return tokens_[currentPos_ - 1];
}

bool TokenStream::isEOF() const {
    return currentPos_ >= tokens_.size();
}

bool TokenStream::hasMore() const {
    return currentPos_ < tokens_.size();
}

size_t TokenStream::position() const {
    return currentPos_;
}

void TokenStream::setPosition(size_t pos) {
    currentPos_ = std::min(pos, tokens_.size());
}

void TokenStream::skipWhitespace() {
    while (hasMore() && (current().isWhitespace() || current().isNewline())) {
        next();
    }
}

void TokenStream::skipNewlines() {
    while (hasMore() && current().isNewline()) {
        next();
    }
}

bool TokenStream::expect(TokenType type) {
    if (current().is(type)) {
        next();
        return true;
    }
    return false;
}

bool TokenStream::expect(const std::vector<TokenType>& types) {
    if (current().isOneOf(types)) {
        next();
        return true;
    }
    return false;
}

bool TokenStream::expectValue(const std::string& value) {
    if (current().value == value) {
        next();
        return true;
    }
    return false;
}

const std::vector<Token>& TokenStream::getAllTokens() const {
    return tokens_;
}

void TokenStream::reset() {
    currentPos_ = 0;
}

// TokenUtil实现
std::string TokenUtil::tokenTypeToString(TokenType type) {
    static const std::unordered_map<TokenType, std::string> typeMap = {
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING, "STRING"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::COMMENT, "COMMENT"},
        {TokenType::KEYWORD_HTML, "KEYWORD_HTML"},
        {TokenType::KEYWORD_HEAD, "KEYWORD_HEAD"},
        {TokenType::KEYWORD_BODY, "KEYWORD_BODY"},
        {TokenType::KEYWORD_DIV, "KEYWORD_DIV"},
        {TokenType::KEYWORD_SPAN, "KEYWORD_SPAN"},
        {TokenType::KEYWORD_TEXT, "KEYWORD_TEXT"},
        {TokenType::KEYWORD_STYLE, "KEYWORD_STYLE"},
        {TokenType::KEYWORD_SCRIPT, "KEYWORD_SCRIPT"},
        {TokenType::KEYWORD_TEMPLATE, "KEYWORD_TEMPLATE"},
        {TokenType::KEYWORD_CUSTOM, "KEYWORD_CUSTOM"},
        {TokenType::KEYWORD_ORIGIN, "KEYWORD_ORIGIN"},
        {TokenType::KEYWORD_IMPORT, "KEYWORD_IMPORT"},
        {TokenType::KEYWORD_NAMESPACE, "KEYWORD_NAMESPACE"},
        {TokenType::KEYWORD_CONFIGURATION, "KEYWORD_CONFIGURATION"},
        {TokenType::KEYWORD_INFO, "KEYWORD_INFO"},
        {TokenType::KEYWORD_EXPORT, "KEYWORD_EXPORT"},
        {TokenType::TEMPLATE_STYLE, "TEMPLATE_STYLE"},
        {TokenType::TEMPLATE_ELEMENT, "TEMPLATE_ELEMENT"},
        {TokenType::TEMPLATE_VAR, "TEMPLATE_VAR"},
        {TokenType::TEMPLATE_HTML, "TEMPLATE_HTML"},
        {TokenType::TEMPLATE_JAVASCRIPT, "TEMPLATE_JAVASCRIPT"},
        {TokenType::TEMPLATE_CHTL, "TEMPLATE_CHTL"},
        {TokenType::TEMPLATE_CJMOD, "TEMPLATE_CJMOD"},
        {TokenType::TEMPLATE_CONFIG, "TEMPLATE_CONFIG"},
        {TokenType::ASSIGN, "ASSIGN"},
        {TokenType::COLON, "COLON"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::QUESTION, "QUESTION"},
        {TokenType::EXCLAMATION, "EXCLAMATION"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::MULTIPLY, "MULTIPLY"},
        {TokenType::DIVIDE, "DIVIDE"},
        {TokenType::MODULO, "MODULO"},
        {TokenType::POWER, "POWER"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::HASH, "HASH"},
        {TokenType::AT, "AT"},
        {TokenType::DOLLAR, "DOLLAR"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::PIPE, "PIPE"},
        {TokenType::TILDE, "TILDE"},
        {TokenType::CARET, "CARET"},
        {TokenType::SINGLE_QUOTE, "SINGLE_QUOTE"},
        {TokenType::DOUBLE_QUOTE, "DOUBLE_QUOTE"},
        {TokenType::BACKTICK, "BACKTICK"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        {TokenType::EOF_TOKEN, "EOF_TOKEN"},
        {TokenType::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = typeMap.find(type);
    return (it != typeMap.end()) ? it->second : "UNKNOWN";
}

TokenType TokenUtil::stringToTokenType(const std::string& str) {
    static const std::unordered_map<std::string, TokenType> stringMap = {
        {"html", TokenType::KEYWORD_HTML},
        {"head", TokenType::KEYWORD_HEAD},
        {"body", TokenType::KEYWORD_BODY},
        {"div", TokenType::KEYWORD_DIV},
        {"span", TokenType::KEYWORD_SPAN},
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"script", TokenType::KEYWORD_SCRIPT},
        {"[Template]", TokenType::KEYWORD_TEMPLATE},
        {"[Custom]", TokenType::KEYWORD_CUSTOM},
        {"[Origin]", TokenType::KEYWORD_ORIGIN},
        {"[Import]", TokenType::KEYWORD_IMPORT},
        {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
        {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
        {"[Info]", TokenType::KEYWORD_INFO},
        {"[Export]", TokenType::KEYWORD_EXPORT},
        {"@Style", TokenType::TEMPLATE_STYLE},
        {"@Element", TokenType::TEMPLATE_ELEMENT},
        {"@Var", TokenType::TEMPLATE_VAR},
        {"@Html", TokenType::TEMPLATE_HTML},
        {"@JavaScript", TokenType::TEMPLATE_JAVASCRIPT},
        {"@Chtl", TokenType::TEMPLATE_CHTL},
        {"@CJmod", TokenType::TEMPLATE_CJMOD},
        {"@Config", TokenType::TEMPLATE_CONFIG},
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
        {"||", TokenType::OR},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"#", TokenType::HASH},
        {"@", TokenType::AT},
        {"$", TokenType::DOLLAR},
        {"&", TokenType::AMPERSAND},
        {"|", TokenType::PIPE},
        {"~", TokenType::TILDE},
        {"^", TokenType::CARET},
        {"'", TokenType::SINGLE_QUOTE},
        {"\"", TokenType::DOUBLE_QUOTE},
        {"`", TokenType::BACKTICK}
    };
    
    auto it = stringMap.find(str);
    return (it != stringMap.end()) ? it->second : TokenType::UNKNOWN;
}

bool TokenUtil::isKeyword(const std::string& str) {
    return getKeywordType(str) != TokenType::UNKNOWN;
}

bool TokenUtil::isOperator(const std::string& str) {
    return getOperatorType(str) != TokenType::UNKNOWN;
}

TokenType TokenUtil::getKeywordType(const std::string& str) {
    return stringToTokenType(str);
}

TokenType TokenUtil::getOperatorType(const std::string& str) {
    return stringToTokenType(str);
}

bool TokenUtil::isOperatorChar(char c) {
    return c == '=' || c == ':' || c == ';' || c == ',' || c == '.' || 
           c == '?' || c == '!' || c == '+' || c == '-' || c == '*' || 
           c == '/' || c == '%' || c == '<' || c == '>' || c == '&' || 
           c == '|' || c == '#' || c == '@' || c == '$' || c == '~' || 
           c == '^' || c == '\'' || c == '"' || c == '`';
}

bool TokenUtil::isIdentifierChar(char c) {
    return std::isalnum(c) || c == '_' || c == '-';
}

bool TokenUtil::isIdentifierStartChar(char c) {
    return std::isalpha(c) || c == '_';
}

bool TokenUtil::isDigit(char c) {
    return std::isdigit(c);
}

bool TokenUtil::isHexDigit(char c) {
    return std::isxdigit(c);
}

bool TokenUtil::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

bool TokenUtil::isNewline(char c) {
    return c == '\n';
}

bool TokenUtil::isStringDelimiter(char c) {
    return c == '\'' || c == '"' || c == '`';
}

bool TokenUtil::isCommentStart(char c, char next) {
    return (c == '/' && next == '/') || (c == '/' && next == '*');
}

int TokenUtil::getOperatorPrecedence(TokenType type) {
    static const std::unordered_map<TokenType, int> precedence = {
        {TokenType::POWER, 15},
        {TokenType::MULTIPLY, 13}, {TokenType::DIVIDE, 13}, {TokenType::MODULO, 13},
        {TokenType::PLUS, 12}, {TokenType::MINUS, 12},
        {TokenType::LESS, 10}, {TokenType::LESS_EQUAL, 10}, 
        {TokenType::GREATER, 10}, {TokenType::GREATER_EQUAL, 10},
        {TokenType::EQUAL, 9}, {TokenType::NOT_EQUAL, 9},
        {TokenType::AND, 5},
        {TokenType::OR, 4},
        {TokenType::QUESTION, 3}, {TokenType::COLON, 3},
        {TokenType::ASSIGN, 2},
        {TokenType::COMMA, 1}
    };
    
    auto it = precedence.find(type);
    return (it != precedence.end()) ? it->second : 0;
}

bool TokenUtil::isLeftAssociative(TokenType type) {
    return type != TokenType::POWER && type != TokenType::QUESTION && type != TokenType::COLON;
}

} // namespace CHTL