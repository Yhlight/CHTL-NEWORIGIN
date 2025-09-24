#pragma once

#include <memory>

namespace CHTL {

// Forward declaration of the context to avoid circular dependencies
class CHTLContext;

/**
 * @class State
 * @brief An abstract base class representing a state in the parsing state machine.
 *
 * Each concrete state will handle a specific part of the CHTL grammar,
 * for example, parsing an element, an attribute, or a style block.
 */
class State {
public:
    virtual ~State() = default;

    // Each state must implement a handle method, which will be the main entry point
    // for processing the input in that state.
    virtual void handle(CHTLContext& context) = 0;
};

} // namespace CHTL
