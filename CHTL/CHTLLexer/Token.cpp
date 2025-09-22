#include "Token.h"

std::string token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::HASH: return "HASH";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::AT: return "AT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::DOT: return "DOT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::FROM: return "FROM";
        case TokenType::CHTL: return "CHTL";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNHANDLED_TOKEN";
    }
}
