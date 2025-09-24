#include "StateMachine.h"

namespace CHTL {

StateMachine::StateMachine() : currentState(nullptr) {}

StateMachine::~StateMachine() = default;

void StateMachine::changeState(std::unique_ptr<State> newState) {
    if (currentState) {
        currentState->onExit(this);
    }
    currentState = std::move(newState);
    if (currentState) {
        currentState->onEnter(this);
    }
}

void StateMachine::update() {
    if (currentState) {
        currentState->onUpdate(this);
    }
}

} // namespace CHTL
