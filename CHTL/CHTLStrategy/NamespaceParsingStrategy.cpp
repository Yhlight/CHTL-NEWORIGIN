#include "NamespaceParsingStrategy.h"
#include "../CHTLParser/CHTLParserContext.h"
#include "../CHTLNode/NamespaceNode.h"
#include "TemplateParsingStrategy.h"
#include "CustomParsingStrategy.h"
#include "ElementParsingStrategy.h"
#include <stdexcept>

namespace CHTL {

std::shared_ptr<BaseNode> NamespaceParsingStrategy::parse(CHTLParserContext* context) {
    context->advance(); // consume '['
    if (context->getCurrentToken().lexeme != "Namespace") {
        throw std::runtime_error("Expected 'Namespace' keyword.");
    }
    context->advance(); // consume 'Namespace'
    if (context->getCurrentToken().type != TokenType::TOKEN_RBRACKET) {
        throw std::runtime_error("Expected ']' after Namespace.");
    }
    context->advance(); // consume ']'

    std::string name = context->getCurrentToken().lexeme;
    context->advance(); // consume namespace name

    auto namespaceNode = std::make_shared<NamespaceNode>(name);

    if (context->getCurrentToken().type != TokenType::TOKEN_LBRACE) {
        throw std::runtime_error("Expected '{' to open namespace block.");
    }
    context->advance(); // consume '{'

    while (context->getCurrentToken().type != TokenType::TOKEN_RBRACE && !context->isAtEnd()) {
        if (context->getCurrentToken().type == TokenType::TOKEN_LBRACKET) {
            const auto& next_lexeme = context->peek(1).lexeme;
            if (next_lexeme == "Template") {
                context->setStrategy(std::make_unique<TemplateParsingStrategy>());
                namespaceNode->addChild(context->runCurrentStrategy());
            } else if (next_lexeme == "Custom") {
                context->setStrategy(std::make_unique<CustomParsingStrategy>());
                namespaceNode->addChild(context->runCurrentStrategy());
            } else if (next_lexeme == "Namespace") {
                context->setStrategy(std::make_unique<NamespaceParsingStrategy>());
                namespaceNode->addChild(context->runCurrentStrategy());
            }
            else {
                throw std::runtime_error("Unexpected definition inside namespace block: " + next_lexeme);
            }
        } else if (context->getCurrentToken().type == TokenType::TOKEN_IDENTIFIER) {
            // Allow top-level elements inside namespaces
            context->setStrategy(std::make_unique<ElementParsingStrategy>());
            namespaceNode->addChild(context->runCurrentStrategy());
        }
        else {
            throw std::runtime_error("Unexpected token in namespace body: " + context->getCurrentToken().lexeme);
        }
    }

    if (context->getCurrentToken().type != TokenType::TOKEN_RBRACE) {
        throw std::runtime_error("Unclosed namespace block.");
    }
    context->advance(); // consume '}'

    return namespaceNode;
}

}