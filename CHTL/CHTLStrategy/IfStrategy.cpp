#include "IfStrategy.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ConditionalNode.h"
#include "../CHTLNode/IfNode.h"
#include "ElementStrategy.h"
#include "TextNodeStrategy.h"
#include <stdexcept>

namespace CHTL {

// Helper to parse the body of a clause (if/else if/else).
// It populates a vector of child nodes.
static void parseClauseChildren(CHTLParser& parser, std::vector<std::unique_ptr<BaseNode>>& children) {
    while (!parser.check(TokenType::R_BRACE) && !parser.isAtEnd()) {
        if (parser.match(TokenType::IF_KEYWORD)) {
            IfStrategy strategy;
            children.push_back(strategy.parse(parser));
        } else if (parser.check(TokenType::IDENTIFIER)) {
            // This is a nested element. ElementStrategy will handle it.
            ElementStrategy strategy;
            children.push_back(strategy.parse(parser));
        } else if (parser.match(TokenType::TEXT_KEYWORD)) {
            TextNodeStrategy strategy;
            children.push_back(strategy.parse(parser));
        }
        else {
            throw std::runtime_error("Unexpected token in conditional block body: " + parser.peek().value);
        }
    }
}


std::unique_ptr<BaseNode> IfStrategy::parse(CHTLParser& parser) {
    // The 'if' keyword that triggered this strategy was already consumed by the caller.
    auto conditionalNode = std::make_unique<ConditionalNode>();

    // --- Parse the first `if` clause ---
    {
        auto ifNode = std::make_unique<IfNode>();
        parser.consume(TokenType::L_BRACE, "Expect '{' after 'if' keyword.");
        parser.consume(TokenType::CONDITION_KEYWORD, "Expect 'condition' keyword in if block.");
        parser.consume(TokenType::COLON, "Expect ':' after 'condition' keyword.");
        ifNode->condition = parser.parseExpression();

        // The spec shows a comma after the condition, so we'll match it optionally.
        if (parser.check(TokenType::COMMA) || parser.check(TokenType::SEMICOLON)) {
            parser.advance();
        }

        parseClauseChildren(parser, ifNode->children);

        parser.consume(TokenType::R_BRACE, "Expect '}' after if block body.");
        conditionalNode->ifClauses.push_back(std::move(ifNode));
    }

    // --- Parse optional `else if` and `else` clauses ---
    while (parser.match(TokenType::ELSE_KEYWORD)) {
        if (parser.match(TokenType::IF_KEYWORD)) {
            // This is an `else if` clause
            auto elseIfNode = std::make_unique<IfNode>();
            parser.consume(TokenType::L_BRACE, "Expect '{' after 'else if' keyword.");
            parser.consume(TokenType::CONDITION_KEYWORD, "Expect 'condition' keyword in else if block.");
            parser.consume(TokenType::COLON, "Expect ':' after 'condition' keyword.");
            elseIfNode->condition = parser.parseExpression();

            if (parser.check(TokenType::COMMA) || parser.check(TokenType::SEMICOLON)) {
                parser.advance();
            }

            parseClauseChildren(parser, elseIfNode->children);

            parser.consume(TokenType::R_BRACE, "Expect '}' after else if block body.");
            conditionalNode->ifClauses.push_back(std::move(elseIfNode));
        } else {
            // This is an `else` clause
            parser.consume(TokenType::L_BRACE, "Expect '{' after 'else' keyword.");

            parseClauseChildren(parser, conditionalNode->elseChildren);

            parser.consume(TokenType::R_BRACE, "Expect '}' after else block body.");
            break; // An `else` must be the final clause in the chain.
        }
    }

    return conditionalNode;
}

}