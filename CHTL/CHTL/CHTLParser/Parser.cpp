#include "Parser.h"
#include <iostream>
#include "../CHTLNode/CommentNode.h"

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    // Prime the pump.
    advance();
}

// --- Public Methods ---

std::unique_ptr<ElementNode> Parser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }
    return parseElement();
}

// --- Private Parsing Methods ---

void Parser::parseAttribute(ElementNode* node) {
    consume(TokenType::IDENTIFIER, "Expect attribute name.");
    std::string key = previous().value;
    consume(TokenType::COLON, "Expect ':' after attribute name.");

    std::string value;
    if (match(TokenType::STRING)) {
        // Quoted strings are consumed as a single token.
        value = previous().value;
    } else {
        // Unadorned literals: consume tokens until a semicolon.
        while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
            if (!value.empty()) {
                value += " "; // Add space between tokens
            }
            advance(); // Consume the token (identifier, comma, etc.)
            value += previous().value;
        }
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    node->addAttribute(key, value);
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    consume(TokenType::IDENTIFIER, "Expect element name.");
    std::string tagName = previous().value;

    auto node = std::make_unique<ElementNode>(tagName);

    consume(TokenType::LBRACE, "Expect '{' after element name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::HASH_COMMENT)) {
            consume(TokenType::HASH_COMMENT, "Should not fail here.");
            node->addChild(std::make_unique<CommentNode>(previous().value));
        } else if (check(TokenType::IDENTIFIER)) {
            if (peek().value == "text") {
                node->addChild(parseTextStatement());
            } else if (peekNext().type == TokenType::COLON) {
                parseAttribute(node.get());
            } else if (peekNext().type == TokenType::LBRACE) {
                node->addChild(parseElement());
            } else {
                hadError = true;
                std::cerr << "Parse error at line " << peek().line << ": Invalid syntax inside element block." << std::endl;
                throw std::runtime_error("Parser error");
            }
        } else {
            hadError = true;
            std::cerr << "Parse error at line " << peek().line << ": Expect property, comment, or element." << std::endl;
            throw std::runtime_error("Parser error");
        }
    }

    consume(TokenType::RBRACE, "Expect '}' after element block.");

    return node;
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    consume(TokenType::IDENTIFIER, "Expect 'text' keyword."); // Consume 'text'

    if (match(TokenType::LBRACE)) { // Block form: text { ... }
        std::string content;
        if (match(TokenType::STRING)) {
            content = previous().value;
        } else {
            // Unadorned literals: consume tokens until a right brace.
            while (!check(TokenType::RBRACE) && !isAtEnd()) {
                if (!content.empty()) {
                    content += " ";
                }
                advance();
                content += previous().value;
            }
        }
        consume(TokenType::RBRACE, "Expect '}' after text block.");
        return std::make_unique<TextNode>(content);
    } else if (match(TokenType::COLON)) { // Property form: text: ...;
        std::string content;
        if (match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
            content = previous().value;
        } else {
            hadError = true;
            std::cerr << "Parse error at line " << peek().line << ": Expect string or identifier for text value." << std::endl;
            throw std::runtime_error("Parser error");
        }
        consume(TokenType::SEMICOLON, "Expect ';' after text property.");
        return std::make_unique<TextNode>(content);
    } else {
        hadError = true;
        std::cerr << "Parse error at line " << peek().line << ": Expect '{' or ':' after 'text' keyword." << std::endl;
        throw std::runtime_error("Parser error");
    }
}


// --- Helper Methods ---

void Parser::advance() {
    previousToken = currentToken;

    if (hasLookahead) {
        currentToken = lookaheadToken;
        hasLookahead = false;
    } else {
        currentToken = lexer.nextToken();
    }

    while (currentToken.type == TokenType::TOKEN_ERROR && !isAtEnd()) {
        std::cerr << "Lexer Error: " << currentToken.value << " on line " << currentToken.line << std::endl;
        hadError = true;
        currentToken = lexer.nextToken();
    }
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    hadError = true;
    std::cerr << "Parse error at line " << peek().line << ": " << message << " Got " << tokenTypeToString(peek().type) << " instead." << std::endl;
    throw std::runtime_error("Parser error");
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

const Token& Parser::peek() {
    return currentToken;
}

const Token& Parser::peekNext() {
    if (!hasLookahead) {
        lookaheadToken = lexer.nextToken();
        hasLookahead = true;
    }
    return lookaheadToken;
}

const Token& Parser::previous() {
    return previousToken;
}
