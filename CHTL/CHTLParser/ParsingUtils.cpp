#include "ParsingUtils.h"
#include "CHTLParserContext.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/PropertyNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

// Helper to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

ParsedCondition parseCondition(const std::string& conditionStr) {
    ParsedCondition result;
    std::vector<std::string> operators = {"==", "!=", ">=", "<=", ">", "<"};
    size_t op_pos = std::string::npos;

    for (const auto& op : operators) {
        size_t pos = conditionStr.find(op);
        if (pos != std::string::npos) {
            op_pos = pos;
            result.op = op;
            break;
        }
    }

    if (op_pos == std::string::npos) {
        throw std::runtime_error("Invalid condition: operator not found in '" + conditionStr + "'.");
    }

    std::string lhs = trim(conditionStr.substr(0, op_pos));
    result.value = trim(conditionStr.substr(op_pos + result.op.length()));

    size_t dot_pos = lhs.find_last_of('.');
    if (dot_pos != std::string::npos) {
        result.selector = trim(lhs.substr(0, dot_pos));
        result.property = trim(lhs.substr(dot_pos + 1));
    } else {
        result.property = lhs; // No selector, just a property
    }

    if (result.property.empty() || result.value.empty()) {
        throw std::runtime_error("Invalid condition: empty property or value in '" + conditionStr + "'.");
    }

    return result;
}

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode) {
    while (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER || context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL) {
        std::string key = context->getCurrentToken().lexeme;
        context->advance();

        if (context->getCurrentToken().type == TokenType::TOKEN_COLON || context->getCurrentToken().type == TokenType::TOKEN_ASSIGN) {
            context->advance(); // consume ':' or '='

            // Check for variable template usage, e.g., ThemeColor(tableColor)
            if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER && context->peek(1).type == TokenType::TOKEN_LPAREN) {
                std::string templateName = context->getCurrentToken().lexeme;
                context->advance(); // consume template name
                context->advance(); // consume '('

                std::string variableName = context->getCurrentToken().lexeme;
                context->advance(); // consume variable name

                if (context->getCurrentToken().type != TokenType::TOKEN_RPAREN) {
                    throw std::runtime_error("Expected ')' to close variable template usage.");
                }
                context->advance(); // consume ')'

                auto propNode = std::make_shared<PropertyNode>(key, ""); // Value will be resolved by generator
                propNode->addChild(std::make_shared<TemplateUsageNode>(templateName, TemplateUsageType::VAR, variableName));
                parentNode->addChild(propNode);

                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                }
            }
            // Regular property value
            else if (context->getCurrentToken().type == TokenType::TOKEN_STRING_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_UNQUOTED_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_NUMERIC_LITERAL ||
                     context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                std::string value = context->getCurrentToken().lexeme;
                context->advance();
                auto propNode = std::make_shared<PropertyNode>(key, value);
                parentNode->addChild(propNode);

                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume ';'
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

}