#include "Parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ElementNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!isAtEnd()) {
        root->addChild(declaration());
    }
    return root;
}

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
    if (!isAtEnd()) current++;
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

std::unique_ptr<BaseNode> Parser::declaration() {
    if (match({TokenType::IDENTIFIER})) {
        return element();
    }
    if (match({TokenType::TEXT})) {
        return text();
    }
    // Other node types (comments, etc.) would be handled here
    // For now, we'll just advance to avoid infinite loops on unhandled tokens
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::element() {
    auto node = std::make_unique<ElementNode>(previous().lexeme);
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if(peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::COLON) {
            node->addAttribute(attribute());
        } else {
            node->addChild(declaration());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

std::unique_ptr<TextNode> Parser::text() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text' keyword.");
    Token content = consume(TokenType::STRING, "Expect string literal inside text block.");
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return std::make_unique<TextNode>(content.lexeme);
}

std::unique_ptr<AttributeNode> Parser::attribute() {
    Token name = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    consume(TokenType::COLON, "Expect ':' after attribute name.");

    if (match({TokenType::STRING, TokenType::IDENTIFIER})) {
        Token value = previous();
        consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
        return std::make_unique<AttributeNode>(name.lexeme, value.lexeme);
    }

    throw std::runtime_error("Expect string or identifier for attribute value.");
}