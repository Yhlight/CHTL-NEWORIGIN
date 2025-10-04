#include "DefinitionState.h"
#include "DefaultState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ValueNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../ExpressionParser/ExpressionParser.h"
#include <map>

// Helper to get the string representation of a token for rebuilding values.
static std::string tokenToString(const Token& token) {
    if (!token.value.empty()) {
        if (token.type == TokenType::CLASS_SELECTOR) return "." + token.value;
        return token.value;
    }
    // Handle tokens that don't have a value, like operators.
    static const std::map<TokenType, std::string> tokenMap = {
        {TokenType::DOT, "."}, {TokenType::PLUS, "+"}, {TokenType::MINUS, "-"},
        {TokenType::STAR, "*"}, {TokenType::SLASH, "/"}, {TokenType::GREATER, ">"},
        {TokenType::LESS, "<"}, {TokenType::GREATER_EQUAL, ">="}, {TokenType::LESS_EQUAL, "<="},
        {TokenType::EQUAL_EQUAL, "=="}, {TokenType::NOT_EQUAL, "!="},
        {TokenType::LOGICAL_AND, "&&"}, {TokenType::LOGICAL_OR, "||"}
    };
    auto it = tokenMap.find(token.type);
    if (it != tokenMap.end()) return it->second;
    return "";
}


DefinitionState::DefinitionState(const std::string& category)
    : category(category), phase(Phase::EXPECT_TYPE), expectingValue(false) {}

void DefinitionState::handle(CHTLParser& parser, Token token) {
    auto parseValue = [&](CHTLParser& p) {
        // Peek ahead to see if this is a complex expression or a simple value.
        bool isExpression = false;
        int i = 0;
        while(true) {
            Token t = p.peek(i);
            if (t.type == TokenType::SEMICOLON || t.type == TokenType::RIGHT_BRACE || t.type == TokenType::END_OF_FILE) break;
            if (t.type == TokenType::PLUS || t.type == TokenType::MINUS || t.type == TokenType::STAR || t.type == TokenType::SLASH || t.type == TokenType::QUESTION_MARK) {
                isExpression = true;
                break;
            }
            i++;
        }

        std::unique_ptr<ExpressionNode> valueNode;
        if(isExpression) {
            ExpressionParser exprParser(p);
            valueNode = exprParser.parse();
        } else {
            std::string valueStr;
            bool firstToken = true;
            while(p.peek().type != TokenType::SEMICOLON && p.peek().type != TokenType::RIGHT_BRACE) {
                if (!firstToken) valueStr += " ";
                valueStr += tokenToString(p.consume());
                firstToken = false;
            }
            valueNode = std::make_unique<ValueNode>(valueStr);
        }

        p.addNode(std::make_unique<StylePropertyNode>(pendingPropertyName, std::move(valueNode)));
        pendingPropertyName.clear();
        expectingValue = false;
    };

    switch (phase) {
        case Phase::EXPECT_TYPE:
            if (token.type == TokenType::AT_SIGN) return; // Consume @
            if (token.type == TokenType::IDENTIFIER) {
                type = token.value;
                phase = Phase::EXPECT_NAME;
            }
            break;

        case Phase::EXPECT_NAME:
            if (token.type == TokenType::IDENTIFIER) {
                name = token.value;
                phase = Phase::EXPECT_BRACE;
            }
            break;

        case Phase::EXPECT_BRACE:
            if (token.type == TokenType::LEFT_BRACE) {
                auto defNode = std::make_unique<TemplateDefinitionNode>(category, type, name);
                auto* defNodePtr = defNode.get();
                parser.addNode(std::move(defNode));
                parser.openScope(defNodePtr);
                phase = Phase::INSIDE_BLOCK;
            }
            break;

        case Phase::INSIDE_BLOCK:
            if (token.type == TokenType::RIGHT_BRACE) {
                parser.closeScope();
                parser.setState(std::make_unique<DefaultState>());
                return;
            }

            if (token.type == TokenType::IDENTIFIER && !expectingValue) {
                pendingPropertyName = token.value;
            } else if (token.type == TokenType::COLON) {
                expectingValue = true;
            } else if (expectingValue) {
                parser.putback(token);
                parseValue(parser);

                if (parser.peek().type == TokenType::SEMICOLON) {
                    parser.consume();
                }
            }
            break;
    }
}