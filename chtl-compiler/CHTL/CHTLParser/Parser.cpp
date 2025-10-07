#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    consume();
    consume(); // Initialize both currentToken and peekToken
}

void Parser::consume() {
    currentToken = peekToken;
    peekToken = lexer.getNextToken();
}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        consume();
    } else {
        std::string err = "Unexpected token: expected " + std::to_string((int)type) +
                          ", got " + std::to_string((int)currentToken.type) +
                          " ('" + currentToken.value + "') at line " + std::to_string(currentToken.line) +
                          ", col " + std::to_string(currentToken.column);
        throw std::runtime_error(err);
    }
}

NodeList Parser::parse() {
    NodeList nodes;
    while(currentToken.type != TokenType::EndOfFile) {
        nodes.push_back(parseStatement());
    }
    return nodes;
}

NodePtr Parser::parseStatement() {
    if (currentToken.type == TokenType::Identifier) {
        if (currentToken.value == "text") {
            return parseText();
        }
        return parseElement();
    }
    return nullptr;
}

NodePtr Parser::parseElement() {
    std::string tagName = currentToken.value;
    eat(TokenType::Identifier);

    auto element = std::make_shared<ElementNode>(tagName);

    eat(TokenType::LeftBrace);

    while (currentToken.type != TokenType::RightBrace && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::Identifier && (peekToken.type == TokenType::Colon || peekToken.type == TokenType::Equal)) {
            parseAttributes(element);
        } else {
            element->children.push_back(parseStatement());
        }
    }

    eat(TokenType::RightBrace);

    return element;
}

void Parser::parseAttributes(std::shared_ptr<ElementNode> element) {
    std::string key = currentToken.value;
    eat(TokenType::Identifier);

    if (currentToken.type == TokenType::Colon || currentToken.type == TokenType::Equal) {
        eat(currentToken.type);
    } else {
        throw std::runtime_error("Expected ':' or '=' for attribute.");
    }

    std::string value;
    if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::StringLiteral) {
        value = currentToken.value;
        eat(currentToken.type);
    } else {
        throw std::runtime_error("Attribute value must be an identifier or a string literal.");
    }

    element->attributes[key] = value;
    eat(TokenType::Semicolon);
}

NodePtr Parser::parseText() {
    eat(TokenType::Identifier); // consume 'text'
    eat(TokenType::LeftBrace);
    std::string content = currentToken.value;
    eat(TokenType::StringLiteral);
    eat(TokenType::RightBrace);

    return std::make_shared<TextNode>(content);
}