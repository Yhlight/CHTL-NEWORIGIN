#include "ConditionalBlockState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ConditionalBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ValueNode.h"
#include <vector>

ConditionalBlockState::ConditionalBlockState(const std::string& type)
    : conditionalType(type), phase(Phase::EXPECT_KEYWORD_CONDITION) {
    if (conditionalType == "else") {
        phase = Phase::EXPECT_PROPERTY; // "else" has no condition
    }
}

void ConditionalBlockState::handle(CHTLParser& parser, Token token) {
    switch (phase) {
        case Phase::EXPECT_KEYWORD_CONDITION:
            if (token.type == TokenType::IDENTIFIER && token.value == "condition") {
                phase = Phase::EXPECT_COLON;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                // Empty if block, just go back
                parser.closeScope();
                parser.setState(std::make_unique<TagState>());
            }
            break;

        case Phase::EXPECT_COLON:
            if (token.type == TokenType::COLON) {
                phase = Phase::EXPECT_EXPRESSION;
            }
            break;

        case Phase::EXPECT_EXPRESSION:
            if (token.type == TokenType::UNKNOWN && token.value == ",") {
                if (!condition.empty() && condition.back() == ' ') {
                    condition.pop_back(); // Trim trailing space
                }
                // We've finished parsing the condition. Update the node.
                BaseNode* currentNode = parser.getCurrentScope();
                if (currentNode && currentNode->getType() == NodeType::ConditionalBlock) {
                    static_cast<ConditionalBlockNode*>(currentNode)->setCondition(condition);
                }
                phase = Phase::EXPECT_PROPERTY;
            } else {
                condition += token.value + " ";
            }
            break;

        case Phase::EXPECT_PROPERTY:
            if (token.type == TokenType::IDENTIFIER) {
                currentProperty = token.value;
                phase = Phase::EXPECT_VALUE;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                parser.closeScope();
                parser.setState(std::make_unique<TagState>());
            }
            break;

        case Phase::EXPECT_VALUE:
            if (token.type == TokenType::COLON) {
                // Explicit colon, wait for value
            } else {
                // Simplified value parsing
                auto valueNode = std::make_unique<ValueNode>(token.value);
                auto propNode = std::make_unique<StylePropertyNode>(currentProperty, std::move(valueNode));
                parser.addNode(std::move(propNode)); // Adds to current scope (the ConditionalBlockNode)

                if (parser.peek().type == TokenType::SEMICOLON) {
                    parser.consume();
                }
                phase = Phase::EXPECT_PROPERTY;
            }
            break;
    }
}