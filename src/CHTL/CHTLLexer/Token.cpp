#include "../../../include/Token.h"

namespace CHTL {

bool Token::isKeyword() const {
    return TokenUtil::isKeyword(value);
}

bool Token::isOperator() const {
    return TokenUtil::isOperator(value);
}

bool Token::isBracket() const {
    return TokenUtil::isBracket(value);
}

bool Token::isLiteral() const {
    return type == TokenType::STRING_LITERAL || type == TokenType::NUMBER_LITERAL;
}

bool Token::isIdentifier() const {
    return type == TokenType::IDENTIFIER;
}

String Token::toString() const {
    return "Token(" + getTypeString() + ", \"" + value + "\", " + 
           std::to_string(line) + ":" + std::to_string(column) + ")";
}

String Token::getTypeString() const {
    return TokenUtil::tokenTypeToString(type);
}

bool Token::operator==(const Token& other) const {
    return type == other.type && value == other.value && 
           line == other.line && column == other.column;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

} // namespace CHTL