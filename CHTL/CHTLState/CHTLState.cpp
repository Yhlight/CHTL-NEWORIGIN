#include "CHTLState.h"

CHTLState::CHTLState() {
    // Start in the global scope
    stateStack.push(ParseState::GLOBAL_SCOPE);
}

void CHTLState::enter(ParseState state) {
    stateStack.push(state);
}

void CHTLState::leave() {
    // Avoid popping the last state to prevent an empty stack
    if (stateStack.size() > 1) {
        stateStack.pop();
    }
}

ParseState CHTLState::getCurrentState() const {
    if (stateStack.empty()) {
        // This should not happen with a correct implementation
        // but as a fallback, return the default state.
        return ParseState::GLOBAL_SCOPE;
    }
    return stateStack.top();
}

bool CHTLState::isInState(ParseState state) const {
    return getCurrentState() == state;
}

size_t CHTLState::getDepth() const {
    return stateStack.size();
}
