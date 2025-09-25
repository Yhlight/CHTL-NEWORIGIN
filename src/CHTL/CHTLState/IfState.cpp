#include "IfState.h"
#include "StatementState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/IfNode.h"
#include "../CHTLNode/ElseIfNode.h"
#include "../CHTLNode/ElseNode.h"
#include "../CHTLNode/StyleValue.h"
#include "StyleBlockState.h" // For parsing expressions

#include <stdexcept>

std::unique_ptr<BaseNode> IfState::handle(Parser& parser) {
    parser.expectToken(TokenType::If); // Consume 'if'

    auto ifNode = std::make_unique<IfNode>();

    parser.expectToken(TokenType::OpenBrace);

    // Parse the condition
    if (parser.currentToken.value == "condition") {
        parser.advanceTokens(); // Consume 'condition'
        parser.expectToken(TokenType::Colon);
        StyleBlockState tempStyleState;
        ifNode->condition = tempStyleState.parseStyleExpression(parser);
        parser.expectToken(TokenType::Semicolon);
    } else {
        throw std::runtime_error("Expected 'condition' in if block.");
    }

    // Parse the body of the if block
    while (parser.currentToken.type != TokenType::CloseBrace) {
        if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Colon) {
            std::string key = parser.currentToken.value;
            parser.advanceTokens(); // consume key
            parser.expectToken(TokenType::Colon);
            StyleBlockState tempStyleState;
            ifNode->properties[key] = tempStyleState.parseStyleExpression(parser);
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Unexpected token in if block body: " + parser.currentToken.value);
        }
    }

    parser.expectToken(TokenType::CloseBrace);

    // Check for and parse else if blocks
    while (parser.currentToken.type == TokenType::ElseIf) {
        parser.advanceTokens(); // Consume 'else if'
        auto elseIfNode = std::make_unique<ElseIfNode>();
        parser.expectToken(TokenType::OpenBrace);

        if (parser.currentToken.value == "condition") {
            parser.advanceTokens(); // Consume 'condition'
            parser.expectToken(TokenType::Colon);
            StyleBlockState tempStyleState;
            elseIfNode->condition = tempStyleState.parseStyleExpression(parser);
            parser.expectToken(TokenType::Semicolon);
        } else {
            throw std::runtime_error("Expected 'condition' in else if block.");
        }

        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Colon) {
                std::string key = parser.currentToken.value;
                parser.advanceTokens(); // consume key
                parser.expectToken(TokenType::Colon);
                StyleBlockState tempStyleState;
                elseIfNode->properties[key] = tempStyleState.parseStyleExpression(parser);
                parser.expectToken(TokenType::Semicolon);
            } else {
                throw std::runtime_error("Unexpected token in else if block body: " + parser.currentToken.value);
            }
        }
        parser.expectToken(TokenType::CloseBrace);
        ifNode->elseIfNodes.push_back(std::move(elseIfNode));
    }

    // Check for and parse an else block
    if (parser.currentToken.type == TokenType::Else) {
        parser.advanceTokens(); // Consume 'else'
        auto elseNode = std::make_unique<ElseNode>();
        parser.expectToken(TokenType::OpenBrace);

        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Identifier && parser.peekToken.type == TokenType::Colon) {
                std::string key = parser.currentToken.value;
                parser.advanceTokens(); // consume key
                parser.expectToken(TokenType::Colon);
                StyleBlockState tempStyleState;
                elseNode->properties[key] = tempStyleState.parseStyleExpression(parser);
                parser.expectToken(TokenType::Semicolon);
            } else {
                throw std::runtime_error("Unexpected token in else block body: " + parser.currentToken.value);
            }
        }
        parser.expectToken(TokenType::CloseBrace);
        ifNode->elseNode = std::move(elseNode);
    }

    // After parsing the if-else if-else chain, we return to the StatementState
    parser.setState(std::make_unique<StatementState>());

    return ifNode;
}