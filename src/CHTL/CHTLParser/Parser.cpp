#include "Parser.h"
#include "../CHTLNode/ElementNode.h"
#include <iostream>
#include <string>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

NodePtr Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!isAtEnd()) {
        try {
            NodePtr child = declaration();
            if (child) {
                root->addChild(std::move(child));
            }
        } catch (const ParseError& error) {
            // A thrown error is a fatal one for a rule, but we can try to recover.
            synchronize();
        }
    }
    return root;
}

// --- Error Handling & Consumption ---

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else if (token.type != TokenType::UNKNOWN) {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
    return ParseError(message);
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}


// --- Grammar Rule Implementations ---

NodePtr Parser::declaration() {
    if (match({TokenType::IDENTIFIER})) {
        return element();
    }

    // If it's not a known declaration, it's a syntax error.
    // We throw, and the main parse loop will catch and synchronize.
    throw error(peek(), "Expect a declaration (e.g., an element).");
}

NodePtr Parser::element() {
    Token tagName = previous();
    auto node = std::make_unique<ElementNode>(tagName.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Any error inside an element will propagate up to the main parse loop.
        node->addChild(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");

    return node;
}


// --- Token Stream Helpers ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::synchronize() {
    advance(); // Consume the token that caused the error to avoid getting stuck.
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            // Resynchronize to the beginning of the next likely declaration
            case TokenType::KEYWORD_TEMPLATE:
            case TokenType::KEYWORD_CUSTOM:
            case TokenType::KEYWORD_ORIGIN:
            case TokenType::KEYWORD_IMPORT:
            case TokenType::KEYWORD_NAMESPACE:
            case TokenType::KEYWORD_CONFIGURATION:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace CHTL
