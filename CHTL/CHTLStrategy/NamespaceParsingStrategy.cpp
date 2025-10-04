#include "NamespaceParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/NamespaceNode.h"
#include "ElementParsingStrategy.h"
#include "TextParsingStrategy.h"
#include "StyleParsingStrategy.h"
#include "TemplateParsingStrategy.h"
#include "CustomParsingStrategy.h"
#include "OriginParsingStrategy.h"
#include "ImportParsingStrategy.h"

namespace CHTL {

std::shared_ptr<BaseNode> NamespaceParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    context->advance(); // consume 'Namespace'
    context->advance(); // consume ']'

    std::string name = context->getCurrentToken().lexeme;
    context->advance(); // consume namespace name

    auto namespaceNode = std::make_shared<NamespaceNode>(name);

    if (context->getCurrentToken().type == TokenType::TOKEN_LBRACE) {
        context->advance(); // consume '{'

        while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
            TokenType currentType = context->getCurrentToken().type;
            if (currentType == TokenType::TOKEN_IDENTIFIER) {
                context->setStrategy(std::make_unique<ElementParsingStrategy>());
                namespaceNode->addChild(context->runCurrentStrategy());
            } else if (currentType == TokenType::TOKEN_LBRACKET) {
                if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Template") {
                    context->setStrategy(std::make_unique<TemplateParsingStrategy>());
                    namespaceNode->addChild(context->runCurrentStrategy());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Custom") {
                    context->setStrategy(std::make_unique<CustomParsingStrategy>());
                    namespaceNode->addChild(context->runCurrentStrategy());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Origin") {
                    context->setStrategy(std::make_unique<OriginParsingStrategy>());
                    namespaceNode->addChild(context->runCurrentStrategy());
                } else if (context->peek(1).type == TokenType::TOKEN_IDENTIFIER && context->peek(1).lexeme == "Import") {
                    context->setStrategy(std::make_unique<ImportParsingStrategy>());
                    namespaceNode->addChild(context->runCurrentStrategy());
                }
            } else {
                context->advance();
            }
        }

        if (context->getCurrentToken().type == TokenType::TOKEN_RBRACE) {
            context->advance(); // consume '}'
        } else {
            // Error: unclosed namespace block
        }
    }

    return namespaceNode;
}

}