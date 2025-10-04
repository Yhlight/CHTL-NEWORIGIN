#include "ConditionalBlockState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ConditionalBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ValueNode.h"
#include <vector>
#include <map>

// Helper to get the string representation of a token for rebuilding the condition
static std::string tokenToString(const Token& token) {
    // This map should cover all tokens that can appear in a condition expression
    static const std::map<TokenType, std::string> tokenMap = {
        {TokenType::DOT, "."},
        {TokenType::PLUS, "+"},
        {TokenType::MINUS, "-"},
        {TokenType::STAR, "*"},
        {TokenType::SLASH, "/"},
        {TokenType::GREATER, ">"},
        {TokenType::LESS, "<"},
        {TokenType::GREATER_EQUAL, ">="},
        {TokenType::LESS_EQUAL, "<="},
        {TokenType::EQUAL_EQUAL, "=="},
        {TokenType::NOT_EQUAL, "!="},
        {TokenType::LOGICAL_AND, "&&"},
        {TokenType::LOGICAL_OR, "||"},
        {TokenType::DOUBLE_LEFT_BRACE, "{{"},
        {TokenType::DOUBLE_RIGHT_BRACE, "}}"}
    };

    if (!token.value.empty()) {
        if (token.type == TokenType::CLASS_SELECTOR) {
            return "." + token.value;
        }
        return token.value;
    }

    auto it = tokenMap.find(token.type);
    if (it != tokenMap.end()) {
        return it->second;
    }

    return ""; // Should not happen for valid expression tokens
}


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
            if (token.type == TokenType::COMMA) {
                // We've finished parsing the condition. Update the node.
                BaseNode* currentNode = parser.getCurrentScope();
                if (currentNode && currentNode->getType() == NodeType::ConditionalBlock) {
                    static_cast<ConditionalBlockNode*>(currentNode)->setCondition(condition);
                }
                phase = Phase::EXPECT_PROPERTY;
            } else {
                condition += tokenToString(token);
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
                auto valueNode = std::make_unique<ValueNode>(token.value);
                auto propNode = std::make_unique<StylePropertyNode>(currentProperty, std::move(valueNode));
                parser.addNode(std::move(propNode));

                if (parser.peek().type == TokenType::SEMICOLON) {
                    parser.consume();
                }
                phase = Phase::EXPECT_PROPERTY;
            }
            break;
    }
}