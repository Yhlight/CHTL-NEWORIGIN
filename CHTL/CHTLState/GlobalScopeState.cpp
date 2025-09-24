#include "GlobalScopeState.h"
#include "CHTLParser/Parser.h" // Include Parser to use its methods
#include "CHTLLexer/Token.h"  // For TokenType
#include <iostream>           // For debug output

namespace CHTL {

GlobalScopeState::GlobalScopeState(Parser* parser) : parser(parser) {}

void GlobalScopeState::onUpdate(StateMachine* sm) {
    // The onUpdate method is the heart of the state. It's called repeatedly
    // by the parser's main loop. Its job is to look at the current token
    // and decide what grammatical rule to apply.

    // For now, we'll implement a very simple rule: if we see an identifier,
    // we assume it's the start of a top-level element declaration.
    if (parser->check(TokenType::IDENTIFIER)) {
        // In a full implementation, we would create an ElementNode and
        // transition the state machine to a new state, like "ParsingElementState".
        // For now, we'll just print a debug message and consume the token.
        std::cout << "DEBUG: GlobalScopeState found identifier: " << parser->peek().lexeme << std::endl;
        parser->advance(); // Consume the identifier
    }
    // We could add other top-level checks here, e.g., for [Template]
    else if (parser->check(TokenType::BRACKETED_KEYWORD)) {
        std::cout << "DEBUG: GlobalScopeState found bracketed keyword: " << parser->peek().lexeme << std::endl;
        parser->advance();
    }
    else {
        // If we see a token we don't recognize at the global scope,
        // we'll advance past it to avoid getting stuck in an infinite loop.
        // A more robust parser would have error reporting here.
        if (!parser->isAtEnd()) {
            parser->advance();
        }
    }
}

} // namespace CHTL
