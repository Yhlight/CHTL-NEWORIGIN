#include "CHTLJSParser.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/StringLiteralNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}

Token& CHTLJSParser::getCurrentToken() {
    if (position >= tokens.size()) {
        static Token eof_token = {TokenType::TOKEN_EOF, ""};
        return eof_token;
    }
    return tokens[position];
}

void CHTLJSParser::advance() {
    if (position < tokens.size()) {
        position++;
    }
}

bool CHTLJSParser::isAtEnd() {
    return position >= tokens.size();
}

std::shared_ptr<BaseNode> CHTLJSParser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }

    Token& current_token = getCurrentToken();

    if (current_token.type == TokenType::TOKEN_DOUBLE_LBRACE) {
        return parseSelector();
    }

    if (current_token.type == TokenType::TOKEN_STRING_LITERAL) {
        auto node = std::make_shared<StringLiteralNode>(current_token.lexeme);
        advance();
        return node;
    }

    // For other tokens, return nullptr for the SaltBridge to handle as raw text.
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseSelector() {
    advance(); // consume '{{'

    std::string selector_str;
    while (!isAtEnd() && getCurrentToken().type != TokenType::TOKEN_DOUBLE_RBRACE) {
        selector_str += getCurrentToken().lexeme;
        advance();
    }

    if (getCurrentToken().type != TokenType::TOKEN_DOUBLE_RBRACE) {
        throw std::runtime_error("Unclosed selector expression. Expected '}}'.");
    }
    advance(); // consume '}}'

    return std::make_shared<SelectorNode>(selector_str);
}

}