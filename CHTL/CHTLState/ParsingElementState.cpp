#include "ParsingElementState.h"
#include "CHTLParser/Parser.h"
#include "GlobalScopeState.h"
#include "CHTLLexer/Token.h"
#include "CHTLNode/ElementNode.h"
#include <utility>
#include <iostream>

namespace CHTL {

ParsingElementState::ParsingElementState(Parser* parser, std::shared_ptr<ElementNode> currentNode)
    : parser(parser), currentNode(std::move(currentNode)) {}

void ParsingElementState::onUpdate(StateMachine* sm) {
    // A nested element is grammatically identical to a top-level one.
    // The easiest way to handle this is to let the GlobalScopeState deal with it,
    // as it already knows how to create elements and push them onto the stack.
    if (parser->check(TokenType::IDENTIFIER) && parser->peekNext().type == TokenType::LEFT_BRACE) {
        sm->changeState(std::make_unique<GlobalScopeState>(parser));
        return;
    }

    // Rule for attributes: IDENTIFIER : VALUE ;
    if (parser->check(TokenType::IDENTIFIER) &&
        (parser->peekNext().type == TokenType::COLON || parser->peekNext().type == TokenType::EQUAL)) {

        Token key = parser->advance();
        parser->advance();

        if (parser->check(TokenType::IDENTIFIER) ||
            parser->check(TokenType::STRING_LITERAL) ||
            parser->check(TokenType::NUMBER_LITERAL)) {

            Token value = parser->advance();
            currentNode->attributes[key.lexeme] = value.lexeme;

            if (!parser->match({TokenType::SEMICOLON})) {
                std::cerr << "Syntax Error: Missing semicolon after attribute value on line " << value.line << std::endl;
            }
            return;
        }
    }

    // Rule for closing brace.
    if (parser->match({TokenType::RIGHT_BRACE})) {
        // We've finished parsing this element, so pop it from the stack
        // to return context to the parent.
        if (!parser->nodeStack.empty()) {
            parser->nodeStack.pop();
        }

        // After popping, the state for the parent element is now active.
        // We can simply re-enter the GlobalScopeState, which will decide what to do next.
        // This handles returning to the global scope or to an outer element's scope.
        sm->changeState(std::make_unique<GlobalScopeState>(parser));
        return;
    }

    // If no rule matches, advance to prevent an infinite loop.
    if (!parser->isAtEnd()) {
        parser->advance();
    }
}

} // namespace CHTL
