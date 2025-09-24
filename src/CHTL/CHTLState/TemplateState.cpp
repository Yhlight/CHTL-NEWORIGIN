#include "TemplateState.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../Util/NodeCloner.h"
#include "StatementState.h" // For parsing nested elements
#include <stdexcept>

std::unique_ptr<BaseNode> TemplateState::handle(Parser& parser) {
    // 1. Expect [Template] or [Custom]
    parser.expectToken(TokenType::OpenBracket);
    bool isCustom = false;
    if (parser.tryExpectKeyword(TokenType::Custom, "KEYWORD_CUSTOM", "Custom")) {
        isCustom = true;
    } else if (!parser.tryExpectKeyword(TokenType::Identifier, "KEYWORD_TEMPLATE", "Template")) {
        throw std::runtime_error("Expected 'Template' or 'Custom' keyword after '['.");
    }
    parser.expectToken(TokenType::CloseBracket);

    // 2. Expect @Type
    parser.expectToken(TokenType::At);
    if (parser.currentToken.type != TokenType::Identifier) throw std::runtime_error("Expected template type (Style, Element, Var) after '@'.");
    std::string templateType = parser.currentToken.value;
    parser.advanceTokens();

    // 3. Parse Template Name
    std::string templateName = parser.currentToken.value;
    parser.expectToken(TokenType::Identifier);

    parser.expectToken(TokenType::OpenBrace);

    // Get the current namespace to register the template under.
    std::string currentNs = parser.getCurrentNamespace();

    // 4. Parse block and register with manager
    if (templateType == "Style") {
        auto styleNode = std::make_unique<StyleTemplateNode>();
        styleNode->isCustom = isCustom;
        while (parser.currentToken.type != TokenType::CloseBrace) {
             if (parser.currentToken.type == TokenType::Inherit || (parser.currentToken.type == TokenType::At && parser.peekToken.value == "Style")) {
                if (parser.currentToken.type == TokenType::Inherit) parser.advanceTokens();
                parser.expectToken(TokenType::At);
                if (parser.currentToken.value != "Style") throw std::runtime_error("Can only inherit from an @Style template.");
                parser.advanceTokens();
                std::string parentName = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
                styleNode->parentNames.push_back(parentName);
            } else {
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);

                if (isCustom && parser.currentToken.type != TokenType::Colon) {
                    styleNode->valuelessProperties.push_back(key);
                    if (parser.currentToken.type == TokenType::Semicolon || parser.currentToken.type == TokenType::Comma) {
                        parser.advanceTokens();
                    }
                    continue;
                }

                parser.expectToken(TokenType::Colon);
                // This is a simplified value parsing, a proper implementation should use the expression parser.
                std::string value;
                while(parser.currentToken.type != TokenType::Semicolon && parser.currentToken.type != TokenType::CloseBrace) {
                    value += parser.currentToken.value + " ";
                    parser.advanceTokens();
                }
                if (!value.empty()) value.pop_back();
                styleNode->styles[key] = value;
                if(parser.currentToken.type == TokenType::Semicolon) parser.advanceTokens();
            }
        }
        parser.templateManager.addStyleTemplate(currentNs, templateName, std::move(styleNode));
    } else if (templateType == "Element") {
        auto elementNode = std::make_unique<ElementTemplateNode>();
        elementNode->isCustom = isCustom;
        StatementState statementParser; // Use a statement parser for the body
        while (parser.currentToken.type != TokenType::CloseBrace) {
             if (parser.currentToken.type == TokenType::Inherit || (parser.currentToken.type == TokenType::At && parser.peekToken.value == "Element")) {
                if (parser.currentToken.type == TokenType::Inherit) parser.advanceTokens();
                parser.expectToken(TokenType::At);
                if (parser.currentToken.value != "Element") throw std::runtime_error("Can only inherit from an @Element template.");
                parser.advanceTokens();
                std::string parentName = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
                ElementTemplateNode* parentTmpl = parser.templateManager.getElementTemplate(currentNs, parentName);
                if (!parentTmpl) throw std::runtime_error("Parent element template not found: " + parentName);
                for (const auto& child : parentTmpl->children) {
                    elementNode->children.push_back(NodeCloner::clone(child.get()));
                }
            } else {
                elementNode->children.push_back(statementParser.handle(parser));
            }
        }
        parser.templateManager.addElementTemplate(currentNs, templateName, std::move(elementNode));
    } else if (templateType == "Var") {
        auto varNode = std::make_unique<VarTemplateNode>();
        varNode->isCustom = isCustom;
        while (parser.currentToken.type != TokenType::CloseBrace) {
            if (parser.currentToken.type == TokenType::Inherit || (parser.currentToken.type == TokenType::At && parser.peekToken.value == "Var")) {
                if (parser.currentToken.type == TokenType::Inherit) parser.advanceTokens();
                parser.expectToken(TokenType::At);
                if (parser.currentToken.value != "Var") throw std::runtime_error("Can only inherit from a @Var template.");
                parser.advanceTokens();
                std::string parentName = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Semicolon);
                VarTemplateNode* parentTmpl = parser.templateManager.getVarTemplate(currentNs, parentName);
                if (!parentTmpl) throw std::runtime_error("Parent var template not found: " + parentName);
                for (const auto& pair : parentTmpl->variables) {
                    varNode->variables[pair.first] = pair.second;
                }
            } else {
                std::string key = parser.currentToken.value;
                parser.expectToken(TokenType::Identifier);
                parser.expectToken(TokenType::Colon);
                // This assumes simple string values for now.
                varNode->variables[key] = parser.currentToken.value;
                parser.expectToken(TokenType::String);
                parser.expectToken(TokenType::Semicolon);
            }
        }
        parser.templateManager.addVarTemplate(currentNs, templateName, std::move(varNode));
    } else {
        throw std::runtime_error("Unknown template type: " + templateType);
    }

    parser.expectToken(TokenType::CloseBrace);
    return nullptr; // Template definitions don't produce a node in the main AST.
}