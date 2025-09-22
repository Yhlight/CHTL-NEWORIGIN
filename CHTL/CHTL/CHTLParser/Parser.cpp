#include "Parser.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<BaseNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!isAtEnd()) {
        root->children.push_back(parseElement());
    }
    return root;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

// For now, a simple error recovery
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (tokens[current - 1].type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::KEYWORD_TEXT:
            case TokenType::IDENTIFIER: // Assuming new element starts
                return;
            default:
                break;
        }
        advance();
    }
}


std::unique_ptr<ElementNode> Parser::parseElement() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(identifier.lexeme);

    if (match({TokenType::LEFT_BRACE})) {
        parseBlock(element.get());
    }

    return element;
}

void Parser::parseBlock(ElementNode* element) {
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::KEYWORD_TEXT) {
            // Handle text: "value"; or text { "value" }
            advance(); // consume 'text'
            if (match({TokenType::COLON, TokenType::EQUALS})) {
                 Token value = consume(TokenType::STRING_LITERAL, "Expect string literal after 'text:'.");
                 element->children.push_back(std::make_unique<TextNode>(value.lexeme));
                 consume(TokenType::SEMICOLON, "Expect ';' after text attribute.");
            } else if (match({TokenType::LEFT_BRACE})) {
                Token value = consume(TokenType::STRING_LITERAL, "Expect string literal inside text block.");
                element->children.push_back(std::make_unique<TextNode>(value.lexeme));
                consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
            }
        } else if (peek().type == TokenType::IDENTIFIER && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS)) {
            parseAttribute(element);
        } else if (peek().type == TokenType::IDENTIFIER) {
            element->children.push_back(parseElement());
        } else {
            // Should not happen on happy path
            advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
}

void Parser::parseAttribute(ElementNode* element) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    if (!match({TokenType::COLON, TokenType::EQUALS})) {
        throw std::runtime_error("Expect ':' or '=' after attribute name.");
    }

    Token value;
    if (match({TokenType::STRING_LITERAL, TokenType::UNQUOTED_LITERAL, TokenType::IDENTIFIER})) {
        value = tokens[current - 1];
    } else {
        throw std::runtime_error("Expect attribute value.");
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");

    element->attributes.push_back(std::make_unique<AttributeNode>(key.lexeme, value.lexeme));
}

} // namespace CHTL
