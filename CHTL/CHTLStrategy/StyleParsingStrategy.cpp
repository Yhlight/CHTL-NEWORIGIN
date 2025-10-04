#include "StyleParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLParser/ParsingUtils.h"

namespace CHTL {

std::shared_ptr<BaseNode> StyleParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume 'style' keyword
    auto styleNode = std::make_shared<StyleNode>();
    auto parentElement = context->getCurrentElement();

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            // Check for a nested rule (e.g., .class, #id, &:hover)
            if ((context->getCurrentToken().type == TokenType::TOKEN_DOT ||
                 context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER && (context->getCurrentToken().lexeme[0] == '#' || context->getCurrentToken().lexeme[0] == '&')) ||
                (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER && context->peek(1).type == TokenType::TOKEN_LBRACE))
            {
                std::string selector;
                if (context->getCurrentToken().type == TokenType::TOKEN_DOT) {
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
                    parseProperties(context, ruleNode); // Reuse property parsing logic
                    if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
                        context->advance(); // consume '}'
                    } else {
                        // Error: unclosed rule block
                    }
                }
                styleNode->addChild(ruleNode);
            }
            // Check for a direct property
            else if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
                // This is a bit of a hack. We create a temporary "root" node to pass to parseProperties.
                auto tempHolder = std::make_shared<ElementNode>("");
                parseProperties(context, tempHolder);
                for(const auto& child : tempHolder->getChildren()) {
                    styleNode->addChild(child);
                }
            } else {
                context->advance(); // Or error
            }
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed style block
        }
    }

    return styleNode;
}

}