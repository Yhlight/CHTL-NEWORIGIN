#pragma once

#include "CHTLState/State.h"
#include <memory>

namespace CHTL {

/**
 * @class CHTLContext
 * @brief Manages the state of the parser and provides a shared context.
 *
 * This class holds the current state of the state machine and allows for
 * transitioning between states. It will also be expanded to hold the token
 * stream, symbol tables, and the AST being built.
 */
class CHTLContext {
public:
    explicit CHTLContext(std::unique_ptr<State> initialState);

    // Set a new state for the context
    void setState(std::unique_ptr<State> newState);

    // The main loop or request handler that delegates to the current state
    void request();

private:
    std::unique_ptr<State> _currentState;
};

} // namespace CHTL
