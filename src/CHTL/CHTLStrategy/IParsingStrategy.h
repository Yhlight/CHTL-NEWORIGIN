#pragma once

#include "../CHTLNode/BaseNode.h"
#include <memory>

// Forward declare Parser to avoid circular dependencies
class Parser;

/**
 * @brief Defines the interface for a parsing strategy.
 *
 * The Strategy pattern is used to encapsulate the logic for parsing a specific
 * language construct (e.g., an HTML element, a style block, a template definition).
 * Each concrete strategy knows how to parse one thing. The ParserState determines
 * which strategy to use based on the current token.
 */
class IParsingStrategy {
public:
    virtual ~IParsingStrategy() = default;

    /**
     * @brief Executes the parsing logic for a specific language construct.
     * @param parser The main parser object, providing access to the token stream and other context.
     * @return A unique_ptr to the new AST node that was created.
     */
    virtual std::unique_ptr<BaseNode> parse(Parser& parser) = 0;
};
