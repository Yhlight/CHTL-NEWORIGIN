#include "IfBehaviorState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../ExpressionParser/ExpressionParser.h"
#include "../CHTLNode/ElementNode.h"

#include <iostream>

IfBehaviorState::IfBehaviorState()
    : phase(Phase::ParsingIfStart), expectingValue(false), headIfNode(nullptr), chainTail(nullptr) {}

void IfBehaviorState::resetForNewBlock() {
    parsedCondition.reset();
    parsedProperties.clear();
    expectingValue = false;
    pendingPropertyName.clear();
}

void IfBehaviorState::handle(CHTLParser &parser, Token token) {
    switch (phase) {
    case Phase::ParsingIfStart:
        if (token.type == TokenType::LEFT_BRACE) {
            phase = Phase::AwaitingConditionKeyword;
        }
        break;

    case Phase::AwaitingConditionKeyword:
        if (token.type == TokenType::KEYWORD_CONDITION) {
            phase = Phase::AwaitingColon;
        }
        break;

    case Phase::AwaitingColon:
        if (token.type == TokenType::COLON) {
            phase = Phase::ParsingCondition;
        }
        break;

    case Phase::ParsingCondition: {
        ExpressionParser exprParser(parser);
        parser.putback(token);
        parsedCondition = exprParser.parse();
        phase = Phase::AwaitingComma;
        break;
    }

    case Phase::AwaitingComma:
        if (token.type == TokenType::COMMA) {
            phase = Phase::ParsingProperties;
        }
        break;

    case Phase::ParsingElseStart:
        if (token.type == TokenType::LEFT_BRACE) {
            phase = Phase::ParsingProperties;
        }
        break;

    case Phase::ParsingProperties: {
        if (token.type == TokenType::RIGHT_BRACE) {
            // A block has finished. Let's create the appropriate node.
            if (!headIfNode) { // This is the first `if` block
                headIfNode = std::make_unique<IfBehaviorNode>(std::move(parsedCondition), std::move(parsedProperties));
                chainTail = headIfNode.get();
            } else {
                if (parsedCondition) { // This is an `else if` block
                    auto elseIfNode = std::make_unique<ElseIfBehaviorNode>(std::move(parsedCondition), std::move(parsedProperties));
                    if (auto* lastIf = dynamic_cast<IfBehaviorNode*>(chainTail)) {
                        lastIf->setElseIfChain(std::move(elseIfNode));
                        chainTail = lastIf->getElseIfChain();
                    } else if (auto* lastElseIf = dynamic_cast<ElseIfBehaviorNode*>(chainTail)) {
                        lastElseIf->setNextElseIf(std::move(elseIfNode));
                        chainTail = lastElseIf->getNextElseIf();
                    }
                } else { // This is an `else` block
                    auto elseNode = std::make_unique<ElseBehaviorNode>(std::move(parsedProperties));
                     if (auto* lastIf = dynamic_cast<IfBehaviorNode*>(chainTail)) {
                        lastIf->setElseNode(std::move(elseNode));
                    } else if (auto* lastElseIf = dynamic_cast<ElseIfBehaviorNode*>(chainTail)) {
                        lastElseIf->setElseNode(std::move(elseNode));
                    }
                    // The chain is now complete, transition back.
                    parser.addNode(std::move(headIfNode));
                    parser.setState(std::make_unique<TagState>());
                    return;
                }
            }
            resetForNewBlock();
            phase = Phase::AwaitingNextLink;
            return; // Return to wait for the next token
        }

        // This logic is similar to StyleState
        if (token.type == TokenType::IDENTIFIER && !expectingValue) {
            pendingPropertyName = token.value;
        } else if (token.type == TokenType::COLON || token.type == TokenType::EQUALS) {
            expectingValue = true;
        } else if (token.type == TokenType::SEMICOLON) {
            pendingPropertyName.clear();
            expectingValue = false;
        } else if (expectingValue) {
            parser.putback(token);
            ExpressionParser exprParser(parser);
            auto valueNode = exprParser.parse();
            parsedProperties.push_back(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
            pendingPropertyName.clear();
            expectingValue = false;
        }
        break;
    }

    case Phase::AwaitingNextLink: {
        if (token.type == TokenType::KEYWORD_ELSE) {
            Token nextToken = parser.peek();
            if (nextToken.type == TokenType::KEYWORD_IF) {
                parser.consume(); // consume 'if'
                phase = Phase::ParsingIfStart;
            } else {
                phase = Phase::ParsingElseStart;
            }
        } else {
            // No `else` or `else if`, so the chain is done.
            parser.putback(token);
            parser.addNode(std::move(headIfNode));
            parser.setState(std::make_unique<TagState>());
        }
        break;
    }
    }
}