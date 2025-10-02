/**
 * @file Token.cpp
 * @brief Implementation of CHTL Token
 */

#include "Token.h"
#include <unordered_map>

namespace CHTL {

std::string Token::toString() const {
    return "Token(" + std::to_string(static_cast<int>(type)) + 
           ", \"" + lexeme + "\", " + 
           std::to_string(line) + ", " + 
           std::to_string(column) + ")";
}

bool isKeyword(TokenType type) {
    return type >= TokenType::KEYWORD_TEXT && type <= TokenType::KEYWORD_EXCEPT;
}

bool isOperator(TokenType type) {
    return (type >= TokenType::COLON && type <= TokenType::DOUBLE_STAR) ||
           (type >= TokenType::LESS && type <= TokenType::OR);
}

bool isLiteral(TokenType type) {
    return type == TokenType::STRING_LITERAL || 
           type == TokenType::UNQUOTED_LITERAL || 
           type == TokenType::NUMBER;
}

TokenType getKeywordType(const std::string& keyword) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"script", TokenType::KEYWORD_SCRIPT},
        {"if", TokenType::KEYWORD_IF},
        {"else", TokenType::KEYWORD_ELSE},
        {"[Template]", TokenType::KEYWORD_TEMPLATE},
        {"[Custom]", TokenType::KEYWORD_CUSTOM},
        {"[Origin]", TokenType::KEYWORD_ORIGIN},
        {"[Import]", TokenType::KEYWORD_IMPORT},
        {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
        {"[Configuration]", TokenType::KEYWORD_CONFIG},
        {"delete", TokenType::KEYWORD_DELETE},
        {"insert", TokenType::KEYWORD_INSERT},
        {"inherit", TokenType::KEYWORD_INHERIT},
        {"from", TokenType::KEYWORD_FROM},
        {"as", TokenType::KEYWORD_AS},
        {"use", TokenType::KEYWORD_USE},
        {"except", TokenType::KEYWORD_EXCEPT},
    };
    
    auto it = keywords.find(keyword);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

} // namespace CHTL
