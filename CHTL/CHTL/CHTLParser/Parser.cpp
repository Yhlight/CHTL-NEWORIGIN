#include "Parser.h"
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    // Prime the pump. Initialize currentToken and previousToken.
    advance();
}

// Main entry point. For now, we assume a single root element.
std::unique_ptr<BaseNode> Parser::parse() {
    if (isAtEnd()) {
        return nullptr;
    }
    // A CHTL file is a sequence of statements (elements).
    // For now, we only parse the first one.
    return parseStatement();
}

// A statement is currently just an element or a text definition.
std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (peek().type == TokenType::IDENTIFIER) {
        if (peek().value == "text") {
            return parseTextStatement();
        }
        return parseElement();
    }

    hadError = true;
    std::cerr << "Parse error at line " << peek().line << ": Expected an element." << std::endl;
    throw std::runtime_error("Parser error");
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    consume(TokenType::IDENTIFIER, "Expect element name.");
    std::string tagName = previous().value;

    auto node = std::make_unique<ElementNode>(tagName);

    consume(TokenType::LBRACE, "Expect '{' after element name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        node->addChild(parseStatement());
    }

    consume(TokenType::RBRACE, "Expect '}' after element block.");

    return node;
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    consume(TokenType::IDENTIFIER, "Expect 'text' keyword."); // Consume 'text'

    if (match(TokenType::LBRACE)) { // Block form: text { "..." }
        consume(TokenType::STRING, "Expect string literal inside text block.");
        std::string content = previous().value;
        consume(TokenType::RBRACE, "Expect '}' after text block.");
        return std::make_unique<TextNode>(content);
    } else if (match(TokenType::COLON)) { // Property form: text: "...";
        consume(TokenType::STRING, "Expect string after 'text:'.");
        std::string content = previous().value;
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
    currentToken = lexer.nextToken();
    // Skip error tokens for now, could implement panic mode recovery later
    while (currentToken.type == TokenType::TOKEN_ERROR) {
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

const Token& Parser::previous() {
    return previousToken;
}
