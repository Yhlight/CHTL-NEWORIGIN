#pragma once

#include "State.h"
#include <memory>
#include <iostream>

namespace CHTL {

class StateMachine {
private:
    std::unique_ptr<State> currentState;

public:
    StateMachine();
    ~StateMachine();

    void changeState(std::unique_ptr<State> newState);

    void update();

    template<typename T>
    T* getCurrentState() const {
        return dynamic_cast<T*>(currentState.get());
    }
};

} // namespace CHTL
