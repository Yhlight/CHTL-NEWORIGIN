#include "StyleState.h"
#include "TagState.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/ClassSelectorNode.h"
#include "../CHTLNode/IdSelectorNode.h"
#include "../CHTLNode/ContextSelectorNode.h"
#include "../CHTLNode/ValueNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../ExpressionParser/ExpressionParser.h"
#include <iostream>
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


StyleState::StyleState() : expectingValue(false), inContextualBlock(false) {}

void StyleState::handle(CHTLParser& parser, Token token) {
    auto parseValue = [&](CHTLParser& p) {
        // Peek ahead to see if this is a complex expression or a simple value.
        bool isExpression = false;
        std::vector<Token> lookaheadBuffer;
        int i = 0;
        while(true) {
            Token t = p.peek(i);
            if (t.type == TokenType::SEMICOLON || t.type == TokenType::RIGHT_BRACE || t.type == TokenType::END_OF_FILE) break;
            if (t.type == TokenType::PLUS || t.type == TokenType::MINUS || t.type == TokenType::STAR || t.type == TokenType::SLASH || t.type == TokenType::QUESTION_MARK) {
                isExpression = true;
                break;
            }
            lookaheadBuffer.push_back(t);
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

    if (inContextualBlock) {
        if (token.type == TokenType::RIGHT_BRACE) {
            parser.closeScope();
            inContextualBlock = false;
        } else if (token.type == TokenType::IDENTIFIER && !expectingValue) {
            pendingPropertyName = token.value;
        } else if (token.type == TokenType::COLON || token.type == TokenType::EQUALS) {
            expectingValue = true;
        } else if (expectingValue) {
            parser.putback(token);
            parseValue(parser);
        }
        return;
    }

    switch (token.type) {
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
        case TokenType::STRING_LITERAL:
        case TokenType::PROPERTY_REFERENCE:
        case TokenType::LEFT_PAREN:
            if (expectingValue) {
                 parser.putback(token);
                 parseValue(parser);
            } else if (token.type == TokenType::IDENTIFIER) {
                pendingPropertyName = token.value;
            }
            break;
        case TokenType::COLON:
        case TokenType::EQUALS:
            expectingValue = true;
            break;
        case TokenType::SEMICOLON:
            pendingPropertyName.clear();
            expectingValue = false;
            break;
        case TokenType::RIGHT_BRACE:
            parser.closeScope();
            parser.setState(std::make_unique<TagState>());
            break;
        case TokenType::AT_SIGN: {
            Token typeToken = parser.consume();
            if (typeToken.type != TokenType::IDENTIFIER) break;
            Token nameToken = parser.consume();
            if (nameToken.type != TokenType::IDENTIFIER) break;
            if (parser.peek().type == TokenType::SEMICOLON) parser.consume();
            auto usageNode = std::make_unique<TemplateUsageNode>(typeToken.value, nameToken.value);
            parser.addNode(std::move(usageNode));
            break;
        }
        case TokenType::CLASS_SELECTOR:
            parser.addNode(std::make_unique<ClassSelectorNode>(token.value));
            break;
        case TokenType::ID_SELECTOR:
            parser.addNode(std::make_unique<IdSelectorNode>(token.value));
            break;
        case TokenType::CONTEXT_SELECTOR:
            pendingContextualSelector = token.value;
            break;
        case TokenType::LEFT_BRACE:
            if (!pendingContextualSelector.empty()) {
                auto contextNode = std::make_unique<ContextSelectorNode>(pendingContextualSelector);
                parser.openScope(contextNode.get());
                parser.addNode(std::move(contextNode));
                pendingContextualSelector.clear();
                inContextualBlock = true;
            }
            break;
        default:
            break;
    }
}