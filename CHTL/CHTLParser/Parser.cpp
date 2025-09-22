#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ElementNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>("root");
    while (!isAtEnd()) {
        try {
            root->addChild(parseDeclaration());
        } catch (const std::runtime_error& e) {
            // In a real compiler, we'd want more sophisticated error recovery (synchronization).
            // For now, we'll print the error and stop.
            std::cerr << "Parse error: " << e.what() << std::endl;
            return root; // Return what we have so far
        }
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::parseDeclaration() {
    if (match({TokenType::TOKEN_IDENTIFIER})) {
        // It could be an element like `div {}` or `text {}`
        if (previous().lexeme == "text") {
            return parseTextNode();
        }
        return parseElement();
    }

    // Skip comments for now in the AST, but a real implementation would add them.
    if (match({TokenType::TOKEN_COMMENT, TokenType::TOKEN_GENERATOR_COMMENT})) {
        // Return a dummy node or handle appropriately
        // For now, let's just create a comment node that the generator can ignore or print
        return std::make_unique<CommentNode>(previous().lexeme, previous().type == TokenType::TOKEN_GENERATOR_COMMENT);
    }

    throw std::runtime_error("Expect a declaration (element, text, etc.). Found token: " + peek().lexeme);
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    Token tagName = previous();
    auto element = std::make_unique<ElementNode>(tagName.lexeme);

    if (match({TokenType::TOKEN_LBRACE})) {
        parseBlock(element.get());
    } else if (match({TokenType::TOKEN_SEMICOLON})) {
        // This handles single-tag elements like `br;`
        // The spec implies all elements can have blocks, but this is good practice.
    } else {
        throw std::runtime_error("Expect '{' or ';' after element name.");
    }

    return element;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'text' keyword.");
    Token content = consume(TokenType::TOKEN_STRING, "Expect string literal inside text block.");
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after text block content.");

    // The lexer includes quotes in the lexeme, let's strip them.
    std::string textValue = content.lexeme.substr(1, content.lexeme.length() - 2);
    return std::make_unique<TextNode>(textValue);
}

void Parser::parseBlock(ElementNode* parent) {
    while (!check(TokenType::TOKEN_RBRACE) && !isAtEnd()) {
        parent->addChild(parseDeclaration());
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after block.");
}

// --- Helper Methods ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
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
    throw std::runtime_error(message + " Got " + peek().lexeme + " instead.");
}
