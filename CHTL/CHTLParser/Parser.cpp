#include "Parser.h"
#include "Util/ExpressionEvaluator.h"
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

    // Only generator comments (#) are part of the AST.
    if (match({TokenType::TOKEN_GENERATOR_COMMENT})) {
        return std::make_unique<CommentNode>(previous().lexeme, true);
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

void Parser::parseAttribute(ElementNode* parent) {
    Token key = advance(); // Consume the identifier

    if (!match({TokenType::TOKEN_COLON, TokenType::TOKEN_EQUALS})) {
        throw std::runtime_error("Expect ':' or '=' after attribute name.");
    }

    Token valueToken = advance();
    if (valueToken.type != TokenType::TOKEN_STRING &&
        valueToken.type != TokenType::TOKEN_IDENTIFIER &&
        valueToken.type != TokenType::TOKEN_NUMBER) {
        throw std::runtime_error("Expect attribute value (string, identifier, or number).");
    }

    std::string value = valueToken.lexeme;
    if (valueToken.type == TokenType::TOKEN_STRING) {
        // Strip quotes from the value
        value = value.substr(1, value.length() - 2);
    }

    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after attribute value.");

    parent->setAttribute(key.lexeme, value);
}

void Parser::parseStyleBlock(ElementNode* parent) {
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'style' keyword.");
    while (!check(TokenType::TOKEN_RBRACE) && !isAtEnd()) {
        Token key = consume(TokenType::TOKEN_IDENTIFIER, "Expect style property name.");

        if (!match({TokenType::TOKEN_COLON, TokenType::TOKEN_EQUALS})) {
            throw std::runtime_error("Expect ':' or '=' after style property name.");
        }

        // Collect tokens for the expression until we hit a semicolon.
        std::vector<Token> valueTokens;
        while (!check(TokenType::TOKEN_SEMICOLON) && !isAtEnd()) {
            valueTokens.push_back(advance());
        }

        if (valueTokens.empty()) {
            throw std::runtime_error("Expect a value for style property '" + key.lexeme + "'.");
        }

        // Evaluate the expression.
        std::string value = ExpressionEvaluator::evaluate(valueTokens);

        consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after style property value.");

        parent->addStyle(key.lexeme, value);
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after style block.");
}

void Parser::parseBlock(ElementNode* parent) {
    while (!check(TokenType::TOKEN_RBRACE) && !isAtEnd()) {
        if (check(TokenType::TOKEN_IDENTIFIER)) {
            if (peek().lexeme == "style") {
                advance(); // consume 'style'
                parseStyleBlock(parent);
            } else if (peekNext().type == TokenType::TOKEN_COLON || peekNext().type == TokenType::TOKEN_EQUALS) {
                parseAttribute(parent);
            } else {
                parent->addChild(parseDeclaration());
            }
        } else {
            // This handles comments and throws errors for other unexpected tokens.
            parent->addChild(parseDeclaration());
        }
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after block.");
}

// --- Helper Methods ---

Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) {
        // Return the EOF token if we're at the end
        return tokens.back();
    }
    return tokens[current + 1];
}

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
