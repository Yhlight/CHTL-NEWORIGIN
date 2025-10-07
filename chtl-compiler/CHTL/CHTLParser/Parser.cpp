#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    currentToken = this->lexer.getNextToken();
}

void Parser::eat(TokenType type) {
    if (currentToken.type == type) {
        currentToken = lexer.getNextToken();
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
        element->children.push_back(parseStatement());
    }

    eat(TokenType::RightBrace);

    return element;
}

NodePtr Parser::parseText() {
    eat(TokenType::Identifier); // consume 'text'
    eat(TokenType::LeftBrace);
    std::string content = currentToken.value;
    eat(TokenType::StringLiteral);
    eat(TokenType::RightBrace);

    return std::make_shared<TextNode>(content);
}