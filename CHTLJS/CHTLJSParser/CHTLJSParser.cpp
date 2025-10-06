#include "CHTLJSParser.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

std::shared_ptr<BaseNode> CHTLJSParser::parse() {
    if (match({TokenType::TOKEN_IDENTIFIER}) && previous().lexeme == "ScriptLoader") {
        if (!match({TokenType::TOKEN_LBRACE})) {
            throw std::runtime_error("Expected '{' after ScriptLoader.");
        }

        auto node = std::make_shared<ScriptLoaderNode>();

        while (!check(TokenType::TOKEN_RBRACE) && !isAtEnd()) {
            if (match({TokenType::TOKEN_IDENTIFIER}) && previous().lexeme == "load") {
                if (!match({TokenType::TOKEN_COLON})) {
                    throw std::runtime_error("Expected ':' after 'load' keyword.");
                }

                // Parse one or more paths
                while (true) {
                    if (match({TokenType::TOKEN_STRING_LITERAL, TokenType::TOKEN_UNQUOTED_LITERAL})) {
                        node->scripts.push_back(previous().lexeme);
                    } else {
                        throw std::runtime_error("Expected a script path (string or unquoted literal).");
                    }

                    if (!check(TokenType::TOKEN_COMMA)) {
                        break; // End of path list for this load statement
                    }

                    // If the comma is followed by another 'load' keyword or the closing brace,
                    // it's a trailing comma, so we break.
                    if (peek(1).type == TokenType::TOKEN_IDENTIFIER || peek(1).type == TokenType::TOKEN_RBRACE) {
                        break;
                    }

                    // Consume the comma and continue parsing paths.
                    advance();
                }
            } else {
                throw std::runtime_error("Expected 'load' keyword inside ScriptLoader block.");
            }

            // Consume optional comma between load statements
            if (check(TokenType::TOKEN_COMMA) && peek(1).type != TokenType::TOKEN_RBRACE) {
                advance();
            }
        }

        if (!match({TokenType::TOKEN_RBRACE})) {
            throw std::runtime_error("Expected '}' to close ScriptLoader block.");
        }

        return node;
    }
    return nullptr;
}

bool CHTLJSParser::isAtEnd() {
    return position >= tokens.size() || peek().type == TokenType::TOKEN_EOF;
}

Token& CHTLJSParser::getCurrentToken() {
    return tokens[position];
}

Token CHTLJSParser::peek(size_t offset) {
    if (position + offset >= tokens.size()) {
        static Token eof{TokenType::TOKEN_EOF, "", 0, 0};
        return eof;
    }
    return tokens[position + offset];
}

Token CHTLJSParser::previous() {
    return tokens[position - 1];
}

Token CHTLJSParser::advance() {
    if (!isAtEnd()) {
        position++;
    }
    return previous();
}

bool CHTLJSParser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool CHTLJSParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

}