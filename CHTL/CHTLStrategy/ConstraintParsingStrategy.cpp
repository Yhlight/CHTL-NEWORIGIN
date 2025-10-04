#include "ConstraintParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/ConstraintNode.h"
#include <stdexcept>

namespace CHTL {

    std::shared_ptr<BaseNode> ConstraintParsingStrategy::parse(CHTLParserContext* context) {
        context->advance(); // consume 'except'
        auto constraintNode = std::make_shared<ConstraintNode>();

        while (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON && !context->isAtEnd()) {
            Constraint constraint;
            Token currentToken = context->getCurrentToken();

            if (currentToken.type == TokenType::TOKEN_AT) {
                context->advance(); // consume '@'
                constraint.type = ConstraintType::TYPE_HTML; // Default to this, can be refined
                constraint.value = context->getCurrentToken().lexeme;
            } else if (currentToken.type == TokenType::TOKEN_LBRACKET) {
                context->advance(); // consume '['
                std::string typeStr = context->getCurrentToken().lexeme; // e.g., "Custom]"
                typeStr.pop_back(); // remove ']'
                if (typeStr == "Custom") {
                    constraint.type = ConstraintType::TYPE_CUSTOM;
                } else if (typeStr == "Template") {
                    constraint.type = ConstraintType::TYPE_TEMPLATE;
                }
                constraint.value = typeStr;
                 if (context->peek(1).type == TokenType::TOKEN_AT) {
                    context->advance(); // consume ']'
                    context->advance(); // consume '@'
                    context->advance(); // consume type
                    constraint.value = context->getCurrentToken().lexeme; // consume name
                    if(typeStr == "Custom") {
                        constraint.type = ConstraintType::CUSTOM;
                    } else if (typeStr == "Template") {
                        constraint.type = ConstraintType::TEMPLATE;
                    }
                }

            }
            else {
                constraint.type = ConstraintType::TAG_NAME;
                constraint.value = currentToken.lexeme;
            }

            constraintNode->addConstraint(constraint);
            context->advance();

            if (context->getCurrentToken().type == TokenType::TOKEN_COMMA) {
                context->advance(); // consume ','
            }
        }

        if (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON) {
            throw std::runtime_error("Expected ';' to terminate except statement.");
        }
        context->advance(); // consume ';'

        return constraintNode;
    }

}