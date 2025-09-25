#pragma once

#include "../CHTLNode/BaseNode.h"
#include <memory>

class Parser; // Forward declaration to avoid circular dependency with Parser.h

// The interface for a parser state. Each concrete state will implement the logic
// for a specific parsing context (e.g., inside an element, inside a style block).
class ParserState {
public:
    virtual ~ParserState() = default;

    // The main handling function for the state. It takes the parser instance
    // as context to access the token stream and to transition to new states.
    // It returns a node for the AST, or nullptr if the state doesn't produce a standalone node.
    virtual std::unique_ptr<BaseNode> handle(Parser& parser) = 0;
};
