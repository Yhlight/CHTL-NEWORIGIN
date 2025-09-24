#include "Context.h"

namespace CHTL {

CHTLContext::CHTLContext(std::unique_ptr<State> initialState)
    : _currentState(std::move(initialState)) {}

void CHTLContext::setState(std::unique_ptr<State> newState) {
    _currentState = std::move(newState);
}

void CHTLContext::request() {
    if (_currentState) {
        _currentState->handle(*this);
    }
}

} // namespace CHTL
