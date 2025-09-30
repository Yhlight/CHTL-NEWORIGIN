#include "CHTLContext.h"
#include "../CHTLState/CHTLState.h"
#include "../CHTLStrategy/CHTLStrategy.h"

namespace CHTL {

CHTLContext::CHTLContext(CHTLState* state, CHTLStrategy* strategy)
    : _state(state), _strategy(strategy) {}

CHTLContext::~CHTLContext() {
    delete _state;
    delete _strategy;
}

void CHTLContext::setState(CHTLState* state) {
    delete _state;
    _state = state;
}

void CHTLContext::setStrategy(CHTLStrategy* strategy) {
    delete _strategy;
    _strategy = strategy;
}

void CHTLContext::request() {
    if (_state) {
        _state->handle(this);
    }
}

void CHTLContext::executeStrategy() {
    if (_strategy) {
        _strategy->execute(this);
    }
}

} // namespace CHTL