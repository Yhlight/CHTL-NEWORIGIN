#pragma once

namespace CHTL {

class StateMachine; // Forward declaration

class State {
public:
    virtual ~State() = default;

    // Called when the state is entered. Optional to implement.
    virtual void onEnter(StateMachine* sm) {}

    // Called on each update tick of the state machine. Must be implemented.
    virtual void onUpdate(StateMachine* sm) = 0;

    // Called when the state is exited. Optional to implement.
    virtual void onExit(StateMachine* sm) {}
};

} // namespace CHTL
