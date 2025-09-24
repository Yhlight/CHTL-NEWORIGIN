#include "GlobalScopeState.h"
#include "CHTLParser/Parser.h"
#include "CHTLLexer/Token.h"
#include "ParsingElementState.h"
#include "CHTLNode/ElementNode.h"
#include <iostream>

namespace CHTL {

GlobalScopeState::GlobalScopeState(Parser* parser) : parser(parser) {}

void GlobalScopeState::onUpdate(StateMachine* sm) {
    // This state should also handle nested elements. The logic is the same.
    if (parser->check(TokenType::IDENTIFIER) && parser->peekNext().type == TokenType::LEFT_BRACE) {
        std::string tagName = parser->advance().lexeme;
        parser->advance(); // Consume '{'

        auto elementNode = std::make_shared<ElementNode>(tagName);

        // Add the new node as a child of the current parent on the stack.
        if (!parser->nodeStack.empty()) {
            parser->nodeStack.top()->addChild(elementNode);
        }

        // Push the new node onto the stack, making it the new current parent.
        parser->nodeStack.push(elementNode);

        // Transition to the state for parsing inside the new element.
        sm->changeState(std::make_unique<ParsingElementState>(parser, elementNode));
        return;
    }

    // If no rule matches, skip the token.
    if (!parser->isAtEnd()) {
        parser->advance();
    }
}

} // namespace CHTL
