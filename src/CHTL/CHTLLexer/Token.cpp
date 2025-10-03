#include "CHTL/CHTLLexer/Token.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// Token implementation
Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {
}

bool Token::isOperator() const {
    switch (type_) {
        case TokenType::COLON:
        case TokenType::EQUALS:
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
        case TokenType::DOT:
        case TokenType::HASH:
        case TokenType::AMPERSAND:
        case TokenType::QUESTION:
        case TokenType::EXCLAMATION:
        case TokenType::ASTERISK:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::CARET:
        case TokenType::PIPE:
        case TokenType::TILDE:
        case TokenType::EQUAL_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS_THAN:
        case TokenType::GREATER_THAN:
        case TokenType::AND_AND:
        case TokenType::OR_OR:
        case TokenType::NOT:
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS:
        case TokenType::PLUS_EQUAL:
        case TokenType::MINUS_EQUAL:
        case TokenType::MULTIPLY_EQUAL:
        case TokenType::DIVIDE_EQUAL:
        case TokenType::MODULO_EQUAL:
        case TokenType::POWER_EQUAL:
            return true;
        default:
            return false;
    }
}

bool Token::isKeyword() const {
    switch (type_) {
        case TokenType::TEXT_KEYWORD:
        case TokenType::STYLE_KEYWORD:
        case TokenType::SCRIPT_KEYWORD:
        case TokenType::TEMPLATE_KEYWORD:
        case TokenType::CUSTOM_KEYWORD:
        case TokenType::IMPORT_KEYWORD:
        case TokenType::NAMESPACE_KEYWORD:
        case TokenType::CONFIG_KEYWORD:
        case TokenType::ORIGIN_KEYWORD:
        case TokenType::USE_KEYWORD:
        case TokenType::INHERIT_KEYWORD:
        case TokenType::DELETE_KEYWORD:
        case TokenType::INSERT_KEYWORD:
        case TokenType::EXCEPT_KEYWORD:
        case TokenType::FROM_KEYWORD:
        case TokenType::AS_KEYWORD:
        case TokenType::AT_KEYWORD:
        case TokenType::VIR_KEYWORD:
        case TokenType::LISTEN_KEYWORD:
        case TokenType::ANIMATE_KEYWORD:
        case TokenType::ROUTER_KEYWORD:
        case TokenType::DELEGATE_KEYWORD:
        case TokenType::SCRIPT_LOADER_KEYWORD:
            return true;
        default:
            return false;
    }
}

bool Token::isLiteral() const {
    switch (type_) {
        case TokenType::LITERAL:
        case TokenType::UNQUOTED_LITERAL:
        case TokenType::STRING_LITERAL:
        case TokenType::SINGLE_QUOTE_LITERAL:
        case TokenType::NUMBER_LITERAL:
        case TokenType::BOOLEAN_LITERAL:
            return true;
        default:
            return false;
    }
}

bool Token::isWhitespace() const {
    switch (type_) {
        case TokenType::WHITESPACE:
        case TokenType::NEWLINE:
        case TokenType::TAB:
        case TokenType::SPACE:
            return true;
        default:
            return false;
    }
}

bool Token::isComment() const {
    switch (type_) {
        case TokenType::LINE_COMMENT:
        case TokenType::BLOCK_COMMENT:
        case TokenType::GENERATOR_COMMENT:
            return true;
        default:
            return false;
    }
}

bool Token::isBracket() const {
    switch (type_) {
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_ANGLE:
        case TokenType::RIGHT_ANGLE:
            return true;
        default:
            return false;
    }
}

bool Token::isComparisonOperator() const {
    switch (type_) {
        case TokenType::EQUAL_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS_THAN:
        case TokenType::GREATER_THAN:
            return true;
        default:
            return false;
    }
}

bool Token::isLogicalOperator() const {
    switch (type_) {
        case TokenType::AND_AND:
        case TokenType::OR_OR:
        case TokenType::NOT:
            return true;
        default:
            return false;
    }
}

bool Token::isArithmeticOperator() const {
    switch (type_) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::ASTERISK:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::CARET:
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS:
        case TokenType::PLUS_EQUAL:
        case TokenType::MINUS_EQUAL:
        case TokenType::MULTIPLY_EQUAL:
        case TokenType::DIVIDE_EQUAL:
        case TokenType::MODULO_EQUAL:
        case TokenType::POWER_EQUAL:
            return true;
        default:
            return false;
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << getTypeString() << ", \"" << value_ << "\", " << line_ << ":" << column_ << ")";
    return oss.str();
}

std::string Token::getTypeString() const {
    static const std::unordered_map<TokenType, std::string> typeNames = {
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::LITERAL, "LITERAL"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::COLON, "COLON"},
        {TokenType::EQUALS, "EQUALS"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::HASH, "HASH"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::QUESTION, "QUESTION"},
        {TokenType::EXCLAMATION, "EXCLAMATION"},
        {TokenType::ASTERISK, "ASTERISK"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::CARET, "CARET"},
        {TokenType::PIPE, "PIPE"},
        {TokenType::TILDE, "TILDE"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_ANGLE, "LEFT_ANGLE"},
        {TokenType::RIGHT_ANGLE, "RIGHT_ANGLE"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LESS_THAN, "LESS_THAN"},
        {TokenType::GREATER_THAN, "GREATER_THAN"},
        {TokenType::AND_AND, "AND_AND"},
        {TokenType::OR_OR, "OR_OR"},
        {TokenType::NOT, "NOT"},
        {TokenType::PLUS_PLUS, "PLUS_PLUS"},
        {TokenType::MINUS_MINUS, "MINUS_MINUS"},
        {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
        {TokenType::MINUS_EQUAL, "MINUS_EQUAL"},
        {TokenType::MULTIPLY_EQUAL, "MULTIPLY_EQUAL"},
        {TokenType::DIVIDE_EQUAL, "DIVIDE_EQUAL"},
        {TokenType::MODULO_EQUAL, "MODULO_EQUAL"},
        {TokenType::POWER_EQUAL, "POWER_EQUAL"},
        {TokenType::TEXT_KEYWORD, "TEXT_KEYWORD"},
        {TokenType::STYLE_KEYWORD, "STYLE_KEYWORD"},
        {TokenType::SCRIPT_KEYWORD, "SCRIPT_KEYWORD"},
        {TokenType::TEMPLATE_KEYWORD, "TEMPLATE_KEYWORD"},
        {TokenType::CUSTOM_KEYWORD, "CUSTOM_KEYWORD"},
        {TokenType::IMPORT_KEYWORD, "IMPORT_KEYWORD"},
        {TokenType::NAMESPACE_KEYWORD, "NAMESPACE_KEYWORD"},
        {TokenType::CONFIG_KEYWORD, "CONFIG_KEYWORD"},
        {TokenType::ORIGIN_KEYWORD, "ORIGIN_KEYWORD"},
        {TokenType::USE_KEYWORD, "USE_KEYWORD"},
        {TokenType::INHERIT_KEYWORD, "INHERIT_KEYWORD"},
        {TokenType::DELETE_KEYWORD, "DELETE_KEYWORD"},
        {TokenType::INSERT_KEYWORD, "INSERT_KEYWORD"},
        {TokenType::EXCEPT_KEYWORD, "EXCEPT_KEYWORD"},
        {TokenType::FROM_KEYWORD, "FROM_KEYWORD"},
        {TokenType::AS_KEYWORD, "AS_KEYWORD"},
        {TokenType::AT_KEYWORD, "AT_KEYWORD"},
        {TokenType::LINE_COMMENT, "LINE_COMMENT"},
        {TokenType::BLOCK_COMMENT, "BLOCK_COMMENT"},
        {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::TAB, "TAB"},
        {TokenType::SPACE, "SPACE"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::SINGLE_QUOTE_LITERAL, "SINGLE_QUOTE_LITERAL"},
        {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
        {TokenType::BOOLEAN_LITERAL, "BOOLEAN_LITERAL"},
        {TokenType::CSS_UNIT, "CSS_UNIT"},
        {TokenType::CSS_FUNCTION, "CSS_FUNCTION"},
        {TokenType::CSS_SELECTOR, "CSS_SELECTOR"},
        {TokenType::CSS_PSEUDO, "CSS_PSEUDO"},
        {TokenType::VIR_KEYWORD, "VIR_KEYWORD"},
        {TokenType::LISTEN_KEYWORD, "LISTEN_KEYWORD"},
        {TokenType::ANIMATE_KEYWORD, "ANIMATE_KEYWORD"},
        {TokenType::ROUTER_KEYWORD, "ROUTER_KEYWORD"},
        {TokenType::DELEGATE_KEYWORD, "DELEGATE_KEYWORD"},
        {TokenType::SCRIPT_LOADER_KEYWORD, "SCRIPT_LOADER_KEYWORD"},
        {TokenType::EOF_TOKEN, "EOF_TOKEN"},
        {TokenType::ERROR_TOKEN, "ERROR_TOKEN"}
    };

    auto it = typeNames.find(type_);
    return it != typeNames.end() ? it->second : "UNKNOWN";
}

bool Token::operator==(const Token& other) const {
    return type_ == other.type_ && value_ == other.value_;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

// TokenStream implementation
TokenStream::TokenStream(const std::vector<Token>& tokens)
    : tokens_(tokens), position_(0), errorPosition_(0), hasError_(false) {
}

TokenStream::TokenStream(std::vector<Token>&& tokens)
    : tokens_(std::move(tokens)), position_(0), errorPosition_(0), hasError_(false) {
}

const Token& TokenStream::current() const {
    if (position_ >= tokens_.size()) {
        static const Token eofToken(TokenType::EOF_TOKEN, "", 0, 0);
        return eofToken;
    }
    return tokens_[position_];
}

const Token& TokenStream::peek(size_t offset) const {
    size_t peekPos = position_ + offset;
    if (peekPos >= tokens_.size()) {
        static const Token eofToken(TokenType::EOF_TOKEN, "", 0, 0);
        return eofToken;
    }
    return tokens_[peekPos];
}

const Token& TokenStream::previous() const {
    if (position_ == 0) {
        static const Token emptyToken(TokenType::ERROR_TOKEN, "", 0, 0);
        return emptyToken;
    }
    return tokens_[position_ - 1];
}

bool TokenStream::advance() {
    if (position_ < tokens_.size()) {
        position_++;
        return true;
    }
    return false;
}

bool TokenStream::hasNext() const {
    return position_ < tokens_.size() - 1;
}

bool TokenStream::hasPrevious() const {
    return position_ > 0;
}

size_t TokenStream::getPosition() const {
    return position_;
}

void TokenStream::setPosition(size_t position) {
    if (position <= tokens_.size()) {
        position_ = position;
    }
}

void TokenStream::reset() {
    position_ = 0;
    hasError_ = false;
}

size_t TokenStream::size() const {
    return tokens_.size();
}

bool TokenStream::empty() const {
    return tokens_.empty();
}

bool TokenStream::hasError() const {
    return hasError_;
}

const Token& TokenStream::getErrorToken() const {
    if (hasError_ && errorPosition_ < tokens_.size()) {
        return tokens_[errorPosition_];
    }
    static const Token emptyToken(TokenType::ERROR_TOKEN, "", 0, 0);
    return emptyToken;
}

} // namespace CHTL