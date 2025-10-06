#include "ConstraintParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ConstraintNode.h"
#include <stdexcept>

namespace CHTL {

    std::shared_ptr<BaseNode> ConstraintParsingStrategy::parse(CHTLParserContext* context) {
        context->advance(); // Consume 'except'
        auto constraintNode = std::make_shared<ConstraintNode>();

        while (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON && !context->isAtEnd()) {
            Constraint constraint;
            Token currentToken = context->getCurrentToken();

            if (currentToken.type == TokenType::TOKEN_LBRACKET) {
                // Handles [Custom], [Template], [Custom] @Type Name, etc.
                context->advance(); // Consume '['
                std::string mainTypeStr = context->getCurrentToken().lexeme; // "Custom" or "Template"
                context->advance(); // Consume mainTypeStr

                if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) {
                    throw std::runtime_error("Expected ']' after " + mainTypeStr);
                }
                context->advance(); // Consume ']'

                if (context->getCurrentToken().type == TokenType::TOKEN_AT) {
                    // Subtype or named constraint
                    context->advance(); // Consume '@'
                    constraint.subType = context->getCurrentToken().lexeme;
                    context->advance(); // Consume subtype (e.g., "Style")

                    if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                        // Named constraint
                        constraint.value = context->getCurrentToken().lexeme;
                        context->advance(); // Consume name
                        constraint.type = (mainTypeStr == "Custom") ? ConstraintType::NAMED_CUSTOM : ConstraintType::NAMED_TEMPLATE;
                    } else {
                        // Subtype constraint
                        constraint.type = (mainTypeStr == "Custom") ? ConstraintType::CUSTOM_SUBTYPE : ConstraintType::TEMPLATE_SUBTYPE;
                    }
                } else {
                    // Any type constraint
                    constraint.type = (mainTypeStr == "Custom") ? ConstraintType::ANY_CUSTOM : ConstraintType::ANY_TEMPLATE;
                }
            } else if (currentToken.type == TokenType::TOKEN_AT) {
                // Handles @Html
                context->advance(); // Consume '@'
                if (context->getCurrentToken().lexeme == "Html") {
                    constraint.type = ConstraintType::ANY_HTML_TAG;
                    context->advance(); // Consume 'Html'
                } else {
                    throw std::runtime_error("Invalid type constraint: @" + context->getCurrentToken().lexeme);
                }
            } else if (currentToken.type == TokenType::TOKEN_IDENTIFIER) {
                // Handles tag name constraint (e.g., except span;)
                constraint.type = ConstraintType::HTML_TAG;
                constraint.value = currentToken.lexeme;
                context->advance(); // Consume the tag name
            } else {
                throw std::runtime_error("Unexpected token in constraint definition: " + currentToken.lexeme);
            }

            constraintNode->addConstraint(constraint);

            if (context->getCurrentToken().type == TokenType::TOKEN_COMMA) {
                context->advance(); // Consume ','
            } else if (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON) {
                throw std::runtime_error("Expected ',' or ';' after constraint definition.");
            }
        }

        if (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON) {
            throw std::runtime_error("Expected ';' to terminate except statement.");
        }
        context->advance(); // Consume ';'

        return constraintNode;
    }

}