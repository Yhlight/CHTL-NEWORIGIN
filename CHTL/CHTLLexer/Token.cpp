#include "Token.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// Token implementation
Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {
}

bool Token::isKeyword() const {
    return (type_ >= TokenType::KEYWORD_USE && type_ <= TokenType::KEYWORD_ELSEIF) ||
           (type_ >= TokenType::CHTL_ELEMENT && type_ <= TokenType::CHTL_CONFIG_CALL);
}

bool Token::isOperator() const {
    return type_ >= TokenType::OPERATOR_COLON && type_ <= TokenType::OPERATOR_QUESTION;
}

bool Token::isLiteral() const {
    return type_ == TokenType::IDENTIFIER ||
           type_ == TokenType::STRING_LITERAL ||
           type_ == TokenType::UNQUOTED_LITERAL ||
           type_ == TokenType::NUMERIC_LITERAL;
}

bool Token::isDelimiter() const {
    return type_ >= TokenType::BRACE_OPEN && type_ <= TokenType::ANGLE_CLOSE;
}

bool Token::isWhitespace() const {
    return type_ == TokenType::WHITESPACE ||
           type_ == TokenType::NEWLINE ||
           type_ == TokenType::TAB;
}

bool Token::isComment() const {
    return type_ == TokenType::LINE_COMMENT ||
           type_ == TokenType::BLOCK_COMMENT ||
           type_ == TokenType::GENERATOR_COMMENT;
}

bool Token::isError() const {
    return type_ == TokenType::ERROR_TOKEN;
}

bool Token::operator==(const Token& other) const {
    return type_ == other.type_ && value_ == other.value_;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << getTypeName() << "(" << value_ << ")";
    if (line_ > 0 || column_ > 0) {
        oss << "[" << line_ << ":" << column_ << "]";
    }
    return oss.str();
}

std::string Token::getTypeName() const {
    return TokenUtils::tokenTypeToString(type_);
}

// Static helper methods
bool Token::isIdentifierStart(char c) {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool Token::isIdentifierPart(char c) {
    return std::isalnum(c) || c == '_' || c == '$' || c == '-';
}

bool Token::isDigit(char c) {
    return std::isdigit(c);
}

bool Token::isHexDigit(char c) {
    return std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Token::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

bool Token::isNewline(char c) {
    return c == '\n';
}

bool Token::isOperatorChar(char c) {
    return c == ':' || c == '=' || c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '%' || c == '&' || c == '|' || c == '!' || c == '<' || c == '>' ||
           c == '?' || c == '.' || c == ',' || c == ';' || c == '^';
}

bool Token::isDelimiterChar(char c) {
    return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' || 
           c == '<' || c == '>' || c == '"' || c == '\'' || c == '`';
}

// TokenStream implementation
TokenStream::TokenStream(const std::vector<Token>& tokens)
    : tokens_(tokens), position_(0), eofToken_(TokenFactory::createEOF(0, 0)) {
}

TokenStream::TokenStream(std::vector<Token>&& tokens)
    : tokens_(std::move(tokens)), position_(0), eofToken_(TokenFactory::createEOF(0, 0)) {
}

const Token& TokenStream::peek(size_t offset) const {
    size_t index = position_ + offset;
    if (index >= tokens_.size()) {
        return eofToken_;
    }
    return tokens_[index];
}

const Token& TokenStream::consume() {
    if (position_ >= tokens_.size()) {
        return eofToken_;
    }
    return tokens_[position_++];
}

bool TokenStream::hasMore() const {
    return position_ < tokens_.size();
}

size_t TokenStream::remaining() const {
    return tokens_.size() - position_;
}

void TokenStream::setPosition(size_t position) {
    position_ = std::min(position, tokens_.size());
}

void TokenStream::reset() {
    position_ = 0;
}

bool TokenStream::match(TokenType type, size_t offset) const {
    const Token& token = peek(offset);
    return token.getType() == type;
}

bool TokenStream::match(const std::string& value, size_t offset) const {
    const Token& token = peek(offset);
    return token.getValue() == value;
}

bool TokenStream::match(TokenType type, const std::string& value, size_t offset) const {
    const Token& token = peek(offset);
    return token.getType() == type && token.getValue() == value;
}

std::vector<Token> TokenStream::getRemainingTokens() const {
    if (position_ >= tokens_.size()) {
        return {};
    }
    return std::vector<Token>(tokens_.begin() + position_, tokens_.end());
}

std::string TokenStream::getSourceText() const {
    std::ostringstream oss;
    for (const auto& token : tokens_) {
        oss << token.getValue();
    }
    return oss.str();
}

// TokenFactory implementation
Token TokenFactory::createIdentifier(const std::string& value, size_t line, size_t column) {
    return Token(TokenType::IDENTIFIER, value, line, column);
}

Token TokenFactory::createStringLiteral(const std::string& value, size_t line, size_t column) {
    return Token(TokenType::STRING_LITERAL, value, line, column);
}

Token TokenFactory::createUnquotedLiteral(const std::string& value, size_t line, size_t column) {
    return Token(TokenType::UNQUOTED_LITERAL, value, line, column);
}

Token TokenFactory::createNumericLiteral(const std::string& value, size_t line, size_t column) {
    return Token(TokenType::NUMERIC_LITERAL, value, line, column);
}

Token TokenFactory::createKeyword(TokenType type, const std::string& value, size_t line, size_t column) {
    return Token(type, value, line, column);
}

Token TokenFactory::createOperator(TokenType type, const std::string& value, size_t line, size_t column) {
    return Token(type, value, line, column);
}

Token TokenFactory::createDelimiter(TokenType type, size_t line, size_t column) {
    std::string value;
    switch (type) {
        case TokenType::BRACE_OPEN: value = "{"; break;
        case TokenType::BRACE_CLOSE: value = "}"; break;
        case TokenType::PAREN_OPEN: value = "("; break;
        case TokenType::PAREN_CLOSE: value = ")"; break;
        case TokenType::BRACKET_OPEN: value = "["; break;
        case TokenType::BRACKET_CLOSE: value = "]"; break;
        case TokenType::ANGLE_OPEN: value = "<"; break;
        case TokenType::ANGLE_CLOSE: value = ">"; break;
        default: value = ""; break;
    }
    return Token(type, value, line, column);
}

Token TokenFactory::createComment(TokenType type, const std::string& value, size_t line, size_t column) {
    return Token(type, value, line, column);
}

Token TokenFactory::createWhitespace(const std::string& value, size_t line, size_t column) {
    TokenType type = TokenType::WHITESPACE;
    if (value == "\n") {
        type = TokenType::NEWLINE;
    } else if (value == "\t") {
        type = TokenType::TAB;
    }
    return Token(type, value, line, column);
}

Token TokenFactory::createError(const std::string& value, size_t line, size_t column) {
    return Token(TokenType::ERROR_TOKEN, value, line, column);
}

Token TokenFactory::createEOF(size_t line, size_t column) {
    return Token(TokenType::EOF_TOKEN, "", line, column);
}

bool TokenFactory::isKeywordType(TokenType type) {
    return (type >= TokenType::KEYWORD_USE && type <= TokenType::KEYWORD_ELSEIF) ||
           (type >= TokenType::CHTL_ELEMENT && type <= TokenType::CHTL_CONFIG_CALL);
}

bool TokenFactory::isOperatorType(TokenType type) {
    return type >= TokenType::OPERATOR_COLON && type <= TokenType::OPERATOR_QUESTION;
}

bool TokenFactory::isDelimiterType(TokenType type) {
    return type >= TokenType::BRACE_OPEN && type <= TokenType::ANGLE_CLOSE;
}

bool TokenFactory::isCommentType(TokenType type) {
    return type == TokenType::LINE_COMMENT ||
           type == TokenType::BLOCK_COMMENT ||
           type == TokenType::GENERATOR_COMMENT;
}

bool TokenFactory::isWhitespaceType(TokenType type) {
    return type == TokenType::WHITESPACE ||
           type == TokenType::NEWLINE ||
           type == TokenType::TAB;
}

// TokenUtils implementation
std::string TokenUtils::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMERIC_LITERAL: return "NUMERIC_LITERAL";
        
        case TokenType::KEYWORD_USE: return "KEYWORD_USE";
        case TokenType::KEYWORD_HTML5: return "KEYWORD_HTML5";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        case TokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case TokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case TokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case TokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case TokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case TokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case TokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case TokenType::KEYWORD_ATTOP: return "KEYWORD_ATTOP";
        case TokenType::KEYWORD_ATBOTTOM: return "KEYWORD_ATBOTTOM";
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case TokenType::KEYWORD_IF: return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE: return "KEYWORD_ELSE";
        case TokenType::KEYWORD_ELSEIF: return "KEYWORD_ELSEIF";
        
        case TokenType::OPERATOR_COLON: return "OPERATOR_COLON";
        case TokenType::OPERATOR_EQUALS: return "OPERATOR_EQUALS";
        case TokenType::OPERATOR_PLUS: return "OPERATOR_PLUS";
        case TokenType::OPERATOR_MINUS: return "OPERATOR_MINUS";
        case TokenType::OPERATOR_MULTIPLY: return "OPERATOR_MULTIPLY";
        case TokenType::OPERATOR_DIVIDE: return "OPERATOR_DIVIDE";
        case TokenType::OPERATOR_MODULO: return "OPERATOR_MODULO";
        case TokenType::OPERATOR_POWER: return "OPERATOR_POWER";
        case TokenType::OPERATOR_AND: return "OPERATOR_AND";
        case TokenType::OPERATOR_OR: return "OPERATOR_OR";
        case TokenType::OPERATOR_NOT: return "OPERATOR_NOT";
        case TokenType::OPERATOR_LESS: return "OPERATOR_LESS";
        case TokenType::OPERATOR_GREATER: return "OPERATOR_GREATER";
        case TokenType::OPERATOR_LESS_EQUAL: return "OPERATOR_LESS_EQUAL";
        case TokenType::OPERATOR_GREATER_EQUAL: return "OPERATOR_GREATER_EQUAL";
        case TokenType::OPERATOR_EQUAL: return "OPERATOR_EQUAL";
        case TokenType::OPERATOR_NOT_EQUAL: return "OPERATOR_NOT_EQUAL";
        case TokenType::OPERATOR_ARROW: return "OPERATOR_ARROW";
        case TokenType::OPERATOR_DOT: return "OPERATOR_DOT";
        case TokenType::OPERATOR_COMMA: return "OPERATOR_COMMA";
        case TokenType::OPERATOR_SEMICOLON: return "OPERATOR_SEMICOLON";
        case TokenType::OPERATOR_QUESTION: return "OPERATOR_QUESTION";
        
        case TokenType::BRACE_OPEN: return "BRACE_OPEN";
        case TokenType::BRACE_CLOSE: return "BRACE_CLOSE";
        case TokenType::PAREN_OPEN: return "PAREN_OPEN";
        case TokenType::PAREN_CLOSE: return "PAREN_CLOSE";
        case TokenType::BRACKET_OPEN: return "BRACKET_OPEN";
        case TokenType::BRACKET_CLOSE: return "BRACKET_CLOSE";
        case TokenType::ANGLE_OPEN: return "ANGLE_OPEN";
        case TokenType::ANGLE_CLOSE: return "ANGLE_CLOSE";
        
        case TokenType::AT_SYMBOL: return "AT_SYMBOL";
        case TokenType::HASH_SYMBOL: return "HASH_SYMBOL";
        case TokenType::DOLLAR_SYMBOL: return "DOLLAR_SYMBOL";
        case TokenType::AMPERSAND_SYMBOL: return "AMPERSAND_SYMBOL";
        case TokenType::PERCENT_SYMBOL: return "PERCENT_SYMBOL";
        
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::TAB: return "TAB";
        
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::ERROR_TOKEN: return "ERROR_TOKEN";
        
        case TokenType::CHTL_ELEMENT: return "CHTL_ELEMENT";
        case TokenType::CHTL_ATTRIBUTE: return "CHTL_ATTRIBUTE";
        case TokenType::CHTL_BEHAVIOR: return "CHTL_BEHAVIOR";
        case TokenType::CHTL_TEMPLATE_CALL: return "CHTL_TEMPLATE_CALL";
        case TokenType::CHTL_CUSTOM_CALL: return "CHTL_CUSTOM_CALL";
        case TokenType::CHTL_VAR_CALL: return "CHTL_VAR_CALL";
        case TokenType::CHTL_ORIGIN_CALL: return "CHTL_ORIGIN_CALL";
        case TokenType::CHTL_IMPORT_CALL: return "CHTL_IMPORT_CALL";
        case TokenType::CHTL_NAMESPACE_CALL: return "CHTL_NAMESPACE_CALL";
        case TokenType::CHTL_CONFIG_CALL: return "CHTL_CONFIG_CALL";
        
        case TokenType::CJMOD_DOUBLE_BRACE_OPEN: return "CJMOD_DOUBLE_BRACE_OPEN";
        case TokenType::CJMOD_DOUBLE_BRACE_CLOSE: return "CJMOD_DOUBLE_BRACE_CLOSE";
        case TokenType::CJMOD_VIRTUAL_OBJECT: return "CJMOD_VIRTUAL_OBJECT";
        case TokenType::CJMOD_FUNCTION_CALL: return "CJMOD_FUNCTION_CALL";
        case TokenType::CJMOD_SELECTOR: return "CJMOD_SELECTOR";
        case TokenType::CJMOD_LISTEN: return "CJMOD_LISTEN";
        case TokenType::CJMOD_ANIMATE: return "CJMOD_ANIMATE";
        case TokenType::CJMOD_ROUTER: return "CJMOD_ROUTER";
        case TokenType::CJMOD_DELEGATE: return "CJMOD_DELEGATE";
        
        default: return "UNKNOWN_TOKEN";
    }
}

TokenType TokenUtils::stringToTokenType(const std::string& str) {
    // This would be implemented with a reverse lookup map
    // For now, return a default
    return TokenType::IDENTIFIER;
}

bool TokenUtils::isValidIdentifier(const std::string& value) {
    if (value.empty()) return false;
    if (!Token::isIdentifierStart(value[0])) return false;
    
    for (size_t i = 1; i < value.length(); ++i) {
        if (!Token::isIdentifierPart(value[i])) return false;
    }
    
    return true;
}

bool TokenUtils::isValidStringLiteral(const std::string& value) {
    if (value.length() < 2) return false;
    
    char quote = value[0];
    if (quote != '"' && quote != '\'' && quote != '`') return false;
    
    return value.back() == quote;
}

bool TokenUtils::isValidNumericLiteral(const std::string& value) {
    if (value.empty()) return false;
    
    bool hasDigit = false;
    bool hasDot = false;
    bool hasE = false;
    bool hasSign = false;
    
    for (size_t i = 0; i < value.length(); ++i) {
        char c = value[i];
        
        if (std::isdigit(c)) {
            hasDigit = true;
        } else if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (c == 'e' || c == 'E') {
            if (hasE) return false;
            hasE = true;
            hasSign = false; // Reset sign flag for exponent
        } else if (c == '+' || c == '-') {
            if (hasSign || (i > 0 && !hasE)) return false;
            hasSign = true;
        } else {
            return false;
        }
    }
    
    return hasDigit;
}

bool TokenUtils::tokensEqual(const Token& a, const Token& b, bool ignorePosition) {
    if (ignorePosition) {
        return a.getType() == b.getType() && a.getValue() == b.getValue();
    }
    return a == b;
}

bool TokenUtils::tokenListEqual(const std::vector<Token>& a, const std::vector<Token>& b, bool ignorePosition) {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (!tokensEqual(a[i], b[i], ignorePosition)) return false;
    }
    
    return true;
}

std::vector<Token> TokenUtils::filterByType(const std::vector<Token>& tokens, TokenType type) {
    std::vector<Token> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
        [type](const Token& token) { return token.getType() == type; });
    return result;
}

std::vector<Token> TokenUtils::filterByTypes(const std::vector<Token>& tokens, const std::vector<TokenType>& types) {
    std::vector<Token> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
        [&types](const Token& token) {
            return std::find(types.begin(), types.end(), token.getType()) != types.end();
        });
    return result;
}

std::vector<Token> TokenUtils::removeWhitespace(const std::vector<Token>& tokens) {
    std::vector<Token> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
        [](const Token& token) { return !token.isWhitespace(); });
    return result;
}

std::vector<Token> TokenUtils::removeComments(const std::vector<Token>& tokens) {
    std::vector<Token> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
        [](const Token& token) { return !token.isComment(); });
    return result;
}

std::string TokenUtils::formatTokenList(const std::vector<Token>& tokens, bool showPosition) {
    std::ostringstream oss;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) oss << " ";
        if (showPosition) {
            oss << tokens[i].toString();
        } else {
            oss << tokens[i].getTypeName() << "(" << tokens[i].getValue() << ")";
        }
    }
    return oss.str();
}

std::string TokenUtils::formatTokenTable(const std::vector<Token>& tokens) {
    std::ostringstream oss;
    oss << "Token Table:\n";
    oss << "Type\t\tValue\t\tLine:Column\n";
    oss << "----\t\t-----\t\t----------\n";
    
    for (const auto& token : tokens) {
        oss << token.getTypeName() << "\t\t" << token.getValue() << "\t\t" 
            << token.getLine() << ":" << token.getColumn() << "\n";
    }
    
    return oss.str();
}

} // namespace CHTL