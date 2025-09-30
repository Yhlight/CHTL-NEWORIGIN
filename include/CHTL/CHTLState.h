#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL State Machine for managing parsing states
 * 
 * The state machine follows the state pattern to handle different parsing contexts
 * such as element parsing, style parsing, script parsing, etc.
 */
class CHTLState {
public:
    enum class StateType {
        INITIAL,
        ELEMENT,
        ATTRIBUTE,
        TEXT_NODE,
        COMMENT,
        STYLE_BLOCK,
        SCRIPT_BLOCK,
        TEMPLATE_DEFINITION,
        CUSTOM_DEFINITION,
        IMPORT_STATEMENT,
        NAMESPACE_DEFINITION,
        CONFIGURATION,
        ORIGIN_BLOCK,
        ERROR
    };

    CHTLState(StateType type, const std::string& context = "");
    virtual ~CHTLState() = default;

    // State management
    virtual bool canTransitionTo(StateType newState) const;
    virtual std::unique_ptr<CHTLState> transitionTo(StateType newState, const std::string& context = "") const;
    virtual void enter();
    virtual void exit();

    // State information
    StateType getType() const { return type_; }
    const std::string& getContext() const { return context_; }
    bool isError() const { return type_ == StateType::ERROR; }

    // Context management
    void setContext(const std::string& context) { context_ = context; }
    void addToContext(const std::string& addition) { context_ += addition; }

protected:
    StateType type_;
    std::string context_;
};

/**
 * @brief State factory for creating appropriate state instances
 */
class CHTLStateFactory {
public:
    static std::unique_ptr<CHTLState> createState(CHTLState::StateType type, const std::string& context = "");
    static bool isValidTransition(CHTLState::StateType from, CHTLState::StateType to);
};

/**
 * @brief State manager for handling state transitions and context
 */
class CHTLStateManager {
public:
    CHTLStateManager();
    ~CHTLStateManager() = default;

    // State management
    void pushState(std::unique_ptr<CHTLState> state);
    void popState();
    CHTLState* getCurrentState() const;
    CHTLState* getPreviousState() const;
    
    // State transitions
    bool transitionTo(CHTLState::StateType newState, const std::string& context = "");
    void resetToInitial();
    
    // State stack management
    size_t getStateStackSize() const;
    bool isInState(CHTLState::StateType stateType) const;
    
    // Error handling
    void setErrorState(const std::string& errorMessage);
    bool hasError() const;
    const std::string& getErrorMessage() const;

private:
    std::vector<std::unique_ptr<CHTLState>> stateStack_;
    std::string errorMessage_;
};

} // namespace CHTL