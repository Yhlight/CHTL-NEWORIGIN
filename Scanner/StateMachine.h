#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <type_traits>

namespace CHTL {

// Forward declarations
template<typename StateEnum>
class StateMachine;

template<typename StateEnum>
class StateTransition;

template<typename StateEnum>
class StateHandler;

// State machine event
template<typename StateEnum>
struct StateEvent {
    StateEnum from;
    StateEnum to;
    std::string trigger;
    std::map<std::string, std::string> data;
    
    StateEvent(StateEnum f, StateEnum t, const std::string& trg = "")
        : from(f), to(t), trigger(trg) {}
};

// State machine context
template<typename StateEnum>
class StateContext {
public:
    StateContext(StateMachine<StateEnum>* machine) : machine_(machine) {}
    
    StateEnum getCurrentState() const;
    void setData(const std::string& key, const std::string& value);
    std::string getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
    void clearData();
    
    // Transition methods
    bool transition(StateEnum to, const std::string& trigger = "");
    bool canTransition(StateEnum to) const;
    
    // Error handling
    void setError(const std::string& error);
    std::string getError() const;
    bool hasError() const;
    void clearError();
    
private:
    StateMachine<StateEnum>* machine_;
    std::map<std::string, std::string> data_;
    std::string error_;
};

// State handler interface
template<typename StateEnum>
class StateHandler {
public:
    virtual ~StateHandler() = default;
    
    virtual void onEnter(StateContext<StateEnum>& context) {}
    virtual void onExit(StateContext<StateEnum>& context) {}
    virtual void onUpdate(StateContext<StateEnum>& context) {}
    virtual bool canEnter(StateContext<StateEnum>& context) const { return true; }
    virtual bool canExit(StateContext<StateEnum>& context) const { return true; }
};

// State transition
template<typename StateEnum>
class StateTransition {
public:
    StateTransition(StateEnum from, StateEnum to, const std::string& trigger = "")
        : from_(from), to_(to), trigger_(trigger) {}
    
    StateEnum getFrom() const { return from_; }
    StateEnum getTo() const { return to_; }
    std::string getTrigger() const { return trigger_; }
    
    bool matches(StateEnum from, StateEnum to, const std::string& trigger = "") const {
        return from_ == from && to_ == to && 
               (trigger_.empty() || trigger_ == trigger);
    }
    
private:
    StateEnum from_;
    StateEnum to_;
    std::string trigger_;
};

// Main state machine class
template<typename StateEnum>
class StateMachine {
public:
    using StateHandlerPtr = std::unique_ptr<StateHandler<StateEnum>>;
    using TransitionCallback = std::function<bool(const StateEvent<StateEnum>&)>;
    
    StateMachine(StateEnum initialState);
    virtual ~StateMachine() = default;
    
    // State management
    StateEnum getCurrentState() const { return currentState_; }
    void setState(StateEnum state);
    bool isInState(StateEnum state) const { return currentState_ == state; }
    
    // Transition management
    bool addTransition(StateEnum from, StateEnum to, const std::string& trigger = "");
    bool removeTransition(StateEnum from, StateEnum to, const std::string& trigger = "");
    bool canTransition(StateEnum to, const std::string& trigger = "") const;
    
    // Handler management
    void setHandler(StateEnum state, StateHandlerPtr handler);
    StateHandler<StateEnum>* getHandler(StateEnum state) const;
    
    // Event handling
    bool trigger(const std::string& triggerName);
    bool transition(StateEnum to, const std::string& trigger = "");
    
    // Callback management
    void setTransitionCallback(TransitionCallback callback);
    void setStateChangeCallback(std::function<void(StateEnum, StateEnum)> callback);
    
    // Context access
    StateContext<StateEnum>& getContext() { return context_; }
    const StateContext<StateEnum>& getContext() const { return context_; }
    
    // Update method for handlers
    void update();
    
    // Reset
    void reset(StateEnum initialState);
    
protected:
    virtual void onStateChanged(StateEnum from, StateEnum to);
    virtual bool onTransition(const StateEvent<StateEnum>& event);
    
private:
    StateEnum currentState_;
    StateContext<StateEnum> context_;
    
    std::map<StateEnum, StateHandlerPtr> handlers_;
    std::vector<StateTransition<StateEnum>> transitions_;
    
    TransitionCallback transitionCallback_;
    std::function<void(StateEnum, StateEnum)> stateChangeCallback_;
    
    bool performTransition(StateEnum to, const std::string& trigger);
};

// Template implementations
template<typename StateEnum>
StateMachine<StateEnum>::StateMachine(StateEnum initialState)
    : currentState_(initialState), context_(this) {
}

template<typename StateEnum>
void StateMachine<StateEnum>::setState(StateEnum state) {
    if (currentState_ != state) {
        StateEnum oldState = currentState_;
        
        // Exit current state
        auto currentHandler = getHandler(currentState_);
        if (currentHandler && currentHandler->canExit(context_)) {
            currentHandler->onExit(context_);
        }
        
        currentState_ = state;
        
        // Enter new state
        auto newHandler = getHandler(currentState_);
        if (newHandler && newHandler->canEnter(context_)) {
            newHandler->onEnter(context_);
        }
        
        onStateChanged(oldState, currentState_);
    }
}

template<typename StateEnum>
bool StateMachine<StateEnum>::addTransition(StateEnum from, StateEnum to, const std::string& trigger) {
    // Check if transition already exists
    for (const auto& transition : transitions_) {
        if (transition.matches(from, to, trigger)) {
            return false;
        }
    }
    
    transitions_.emplace_back(from, to, trigger);
    return true;
}

template<typename StateEnum>
bool StateMachine<StateEnum>::removeTransition(StateEnum from, StateEnum to, const std::string& trigger) {
    auto it = std::find_if(transitions_.begin(), transitions_.end(),
        [from, to, trigger](const StateTransition<StateEnum>& t) {
            return t.matches(from, to, trigger);
        });
    
    if (it != transitions_.end()) {
        transitions_.erase(it);
        return true;
    }
    
    return false;
}

template<typename StateEnum>
bool StateMachine<StateEnum>::canTransition(StateEnum to, const std::string& trigger) const {
    for (const auto& transition : transitions_) {
        if (transition.matches(currentState_, to, trigger)) {
            return true;
        }
    }
    return false;
}

template<typename StateEnum>
void StateMachine<StateEnum>::setHandler(StateEnum state, StateHandlerPtr handler) {
    handlers_[state] = std::move(handler);
}

template<typename StateEnum>
StateHandler<StateEnum>* StateMachine<StateEnum>::getHandler(StateEnum state) const {
    auto it = handlers_.find(state);
    return (it != handlers_.end()) ? it->second.get() : nullptr;
}

template<typename StateEnum>
bool StateMachine<StateEnum>::trigger(const std::string& triggerName) {
    for (const auto& transition : transitions_) {
        if (transition.getFrom() == currentState_ && 
            transition.getTrigger() == triggerName) {
            return performTransition(transition.getTo(), triggerName);
        }
    }
    return false;
}

template<typename StateEnum>
bool StateMachine<StateEnum>::transition(StateEnum to, const std::string& trigger) {
    if (canTransition(to, trigger)) {
        return performTransition(to, trigger);
    }
    return false;
}

template<typename StateEnum>
void StateMachine<StateEnum>::setTransitionCallback(TransitionCallback callback) {
    transitionCallback_ = std::move(callback);
}

template<typename StateEnum>
void StateMachine<StateEnum>::setStateChangeCallback(std::function<void(StateEnum, StateEnum)> callback) {
    stateChangeCallback_ = std::move(callback);
}

template<typename StateEnum>
void StateMachine<StateEnum>::update() {
    auto handler = getHandler(currentState_);
    if (handler) {
        handler->onUpdate(context_);
    }
}

template<typename StateEnum>
void StateMachine<StateEnum>::reset(StateEnum initialState) {
    currentState_ = initialState;
    context_.clearData();
    context_.clearError();
    
    auto handler = getHandler(currentState_);
    if (handler) {
        handler->onEnter(context_);
    }
}

template<typename StateEnum>
void StateMachine<StateEnum>::onStateChanged(StateEnum from, StateEnum to) {
    if (stateChangeCallback_) {
        stateChangeCallback_(from, to);
    }
}

template<typename StateEnum>
bool StateMachine<StateEnum>::onTransition(const StateEvent<StateEnum>& event) {
    if (transitionCallback_) {
        return transitionCallback_(event);
    }
    return true;
}

template<typename StateEnum>
bool StateMachine<StateEnum>::performTransition(StateEnum to, const std::string& trigger) {
    StateEvent<StateEnum> event(currentState_, to, trigger);
    
    if (!onTransition(event)) {
        return false;
    }
    
    setState(to);
    return true;
}

// StateContext implementations
template<typename StateEnum>
StateEnum StateContext<StateEnum>::getCurrentState() const {
    return machine_->getCurrentState();
}

template<typename StateEnum>
void StateContext<StateEnum>::setData(const std::string& key, const std::string& value) {
    data_[key] = value;
}

template<typename StateEnum>
std::string StateContext<StateEnum>::getData(const std::string& key) const {
    auto it = data_.find(key);
    return (it != data_.end()) ? it->second : "";
}

template<typename StateEnum>
bool StateContext<StateEnum>::hasData(const std::string& key) const {
    return data_.find(key) != data_.end();
}

template<typename StateEnum>
void StateContext<StateEnum>::clearData() {
    data_.clear();
}

template<typename StateEnum>
bool StateContext<StateEnum>::transition(StateEnum to, const std::string& trigger) {
    return machine_->transition(to, trigger);
}

template<typename StateEnum>
bool StateContext<StateEnum>::canTransition(StateEnum to) const {
    return machine_->canTransition(to);
}

template<typename StateEnum>
void StateContext<StateEnum>::setError(const std::string& error) {
    error_ = error;
}

template<typename StateEnum>
std::string StateContext<StateEnum>::getError() const {
    return error_;
}

template<typename StateEnum>
bool StateContext<StateEnum>::hasError() const {
    return !error_.empty();
}

template<typename StateEnum>
void StateContext<StateEnum>::clearError() {
    error_.clear();
}

} // namespace CHTL