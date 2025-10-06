#include "StyleParsingStrategy.h"
#include "IfParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLParser/ParsingUtils.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> StyleParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume 'style' keyword

    if (context->getCurrentToken().type == TokenType::TOKEN_COLON) {
        throw std::runtime_error("Explicit inheritance is not allowed in anonymous style templates.");
    }

    auto styleNode = std::make_shared<StyleNode>();
    auto parentElement = context->getCurrentElement();

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            TokenType currentType = context->getCurrentToken().type;

            if (currentType == TokenType::TOKEN_IF || currentType == TokenType::TOKEN_ELSE) {
                context->setStrategy(std::make_unique<IfParsingStrategy>());
                styleNode->addChild(context->runCurrentStrategy());
            }
            // Check for a nested rule (e.g., .class, #id, &:hover)
            else if (currentType == TokenType::TOKEN_DOT ||
                (currentType == TokenType::TOKEN_IDENTIFIER && (context->getCurrentToken().lexeme[0] == '#' || context->getCurrentToken().lexeme[0] == '&')) ||
                (currentType == TokenType::TOKEN_IDENTIFIER && context->peek(1).type == TokenType::TOKEN_LBRACE))
            {
                std::string selector;
                if (currentType == TokenType::TOKEN_DOT) {
                    context->advance(); // consume '.'
                    selector = "." + context->getCurrentToken().lexeme;
                    if (parentElement && !parentElement->hasAttribute("class")) {
                        parentElement->setAttribute("class", context->getCurrentToken().lexeme);
                    }
                } else if (context->getCurrentToken().lexeme[0] == '#') {
                     selector = context->getCurrentToken().lexeme;
                     if (parentElement && !parentElement->hasAttribute("id")) {
                        parentElement->setAttribute("id", selector.substr(1));
                    }
                } else {
                    selector = context->getCurrentToken().lexeme;
                }
                context->advance(); // consume selector identifier

                auto ruleNode = std::make_shared<RuleNode>(selector);

                if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
                    context->advance(); // consume '{'
                    parseProperties(context, ruleNode);
                    if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
                        context->advance(); // consume '}'
                    }
                }
                styleNode->addChild(ruleNode);
            }
            // Check for a direct property
            else if (currentType == TokenType::TOKEN_IDENTIFIER || currentType == TokenType::TOKEN_UNQUOTED_LITERAL) {
                std::string key = context->getCurrentToken().lexeme;
                context->advance(); // consume key

                if (context->getCurrentToken().type != TokenType::TOKEN_COLON && context->getCurrentToken().type != TokenType::TOKEN_ASSIGN) {
                    throw std::runtime_error("Expected ':' or '=' after property key '" + key + "' in style block.");
                }
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
                    styleNode->addChild(propNode);

                } else {
                    // It's a regular property value
                    std::string value;
                    while (context->getCurrentToken().type != TokenType::TOKEN_SEMICOLON &&
                           context->getCurrentToken().type != TokenType::TOKEN_RBRACE &&
                           !context->isAtEnd()) {
                        value += context->getCurrentToken().lexeme;
                        const auto& next_token = context->peek(1);
                        if (next_token.type != TokenType::TOKEN_SEMICOLON && next_token.type != TokenType::TOKEN_RBRACE && next_token.type != TokenType::TOKEN_EOF) {
                            value += " ";
                        }
                        context->advance();
                    }
                    styleNode->addChild(std::make_shared<PropertyNode>(key, value));
                }

                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance(); // consume optional semicolon
                }
            }
            // Check for template usage
            else if (currentType == TokenType::TOKEN_AT) {
                context->advance(); // consume '@'

                std::string itemTypeStr = context->getCurrentToken().lexeme;
                context->advance();

                TemplateUsageType usageType;
                if (itemTypeStr == "Style") {
                    usageType = TemplateUsageType::STYLE;
                } else {
                    throw std::runtime_error("Only @Style templates can be used in this context. Found: @" + itemTypeStr);
                }

                std::string templateName = context->getCurrentToken().lexeme;
                context->advance();

                if (context->getCurrentToken().type == TokenType::TOKEN_SEMICOLON) {
                    context->advance();
                } else {
                    throw std::runtime_error("Expected ';' after template usage.");
                }

                styleNode->addChild(std::make_shared<TemplateUsageNode>(templateName, usageType));
            }
            else {
                context->advance();
            }
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            throw std::runtime_error("Unclosed style block.");
        }
    }

    return styleNode;
}

}