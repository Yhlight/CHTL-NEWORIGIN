#include <iostream>
#include <cassert>
#include "../Scanner/StateMachine.h"

using namespace CHTL;

// Test state enum
enum class TestState {
    INIT,
    PROCESSING,
    COMPLETE,
    ERROR
};

// Test state handler
class TestStateHandler : public StateHandler<TestState> {
public:
    void onEnter(StateContext<TestState>& context) override {
        context.setData("status", "entered");
    }
    
    void onUpdate(StateContext<TestState>& context) override {
        // Test update logic
    }
};

int main() {
    std::cout << "Running CHTL State Machine Tests..." << std::endl;
    
    // Test state machine creation
    StateMachine<TestState> stateMachine(TestState::INIT);
    assert(stateMachine.getCurrentState() == TestState::INIT);
    std::cout << "✓ State machine creation test passed" << std::endl;
    
    // Test state handler registration
    stateMachine.setHandler(TestState::INIT, std::make_unique<TestStateHandler>());
    stateMachine.setHandler(TestState::PROCESSING, std::make_unique<TestStateHandler>());
    stateMachine.setHandler(TestState::COMPLETE, std::make_unique<TestStateHandler>());
    assert(stateMachine.getHandler(TestState::INIT) != nullptr);
    std::cout << "✓ State handler registration test passed" << std::endl;
    
    // Test transition registration
    bool transitionAdded = stateMachine.addTransition(TestState::INIT, TestState::PROCESSING, "start");
    assert(transitionAdded);
    assert(stateMachine.canTransition(TestState::PROCESSING, "start"));
    std::cout << "✓ Transition registration test passed" << std::endl;
    
    // Test state transition
    bool transitioned = stateMachine.transition(TestState::PROCESSING, "start");
    assert(transitioned);
    assert(stateMachine.getCurrentState() == TestState::PROCESSING);
    std::cout << "✓ State transition test passed" << std::endl;
    
    // Test context data
    StateContext<TestState>& context = stateMachine.getContext();
    assert(context.hasData("status"));
    assert(context.getData("status") == "entered");
    std::cout << "✓ Context data test passed" << std::endl;
    
    // Test trigger
    stateMachine.addTransition(TestState::PROCESSING, TestState::COMPLETE, "finish");
    bool triggered = stateMachine.trigger("finish");
    assert(triggered);
    assert(stateMachine.getCurrentState() == TestState::COMPLETE);
    std::cout << "✓ Trigger test passed" << std::endl;
    
    // Test error handling
    context.setError("Test error");
    assert(context.hasError());
    assert(context.getError() == "Test error");
    context.clearError();
    assert(!context.hasError());
    std::cout << "✓ Error handling test passed" << std::endl;
    
    std::cout << "All state machine tests passed!" << std::endl;
    return 0;
}