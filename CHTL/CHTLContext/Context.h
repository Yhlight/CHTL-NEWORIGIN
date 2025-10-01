#pragma once

#include "../CHTLState/State.h"
#include <memory>

class Context {
public:
    Context(std::unique_ptr<State> state) : state(std::move(state)) {}

    void transitionTo(std::unique_ptr<State> newState) {
        state = std::move(newState);
    }

    void request() {
        state->handle(*this);
    }

private:
    std::unique_ptr<State> state;
};