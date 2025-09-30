#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <memory>
#include <string>
#include <functional>

namespace CHTL {

// Forward declarations
class CHTLStateManager;

// State Type Enumeration
enum class StateType {
    INITIAL,
    ELEMENT,
    ATTRIBUTE,
    TEXT,
    COMMENT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    CONFIG,
    NAMESPACE,
    OPERATOR,
    ERROR
};

// Base State Class
class CHTLState {
public:
    virtual ~CHTLState() = default;
    
    // Process a token and return the next state
    virtual std::unique_ptr<CHTLState> processToken(const std::string& token) = 0;
    
    // Get the type of this state
    virtual StateType getStateType() const = 0;
    
    // Check if this is a terminal state
    virtual bool isTerminal() const = 0;
    
    // Get the name of this state for debugging
    virtual std::string getStateName() const = 0;
};

// Concrete State Classes
class InitialState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::INITIAL; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Initial"; }
};

class ElementState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::ELEMENT; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Element"; }
};

class AttributeState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::ATTRIBUTE; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Attribute"; }
};

class TextState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::TEXT; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Text"; }
};

class CommentState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::COMMENT; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Comment"; }
};

class StyleState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::STYLE; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Style"; }
};

class ScriptState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::SCRIPT; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Script"; }
};

class TemplateState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::TEMPLATE; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Template"; }
};

class CustomState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::CUSTOM; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Custom"; }
};

class OriginState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::ORIGIN; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Origin"; }
};

class ImportState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::IMPORT; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Import"; }
};

class ConfigState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::CONFIG; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Config"; }
};

class NamespaceState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::NAMESPACE; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Namespace"; }
};

class OperatorState : public CHTLState {
public:
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::OPERATOR; }
    bool isTerminal() const override { return false; }
    std::string getStateName() const override { return "Operator"; }
};

class ErrorState : public CHTLState {
private:
    std::string errorMessage_;
    
public:
    ErrorState(const std::string& message);
    std::unique_ptr<CHTLState> processToken(const std::string& token) override;
    StateType getStateType() const override { return StateType::ERROR; }
    bool isTerminal() const override { return true; }
    std::string getStateName() const override { return "Error"; }
    std::string getErrorMessage() const { return errorMessage_; }
};

// State Factory
class CHTLStateFactory {
public:
    static std::unique_ptr<CHTLState> createState(StateType type);
};

// State Manager
class CHTLStateManager {
public:
    CHTLStateManager();
    
    // Initialize the state machine
    void initialize();
    
    // Process a token through the state machine
    bool processToken(const std::string& token);
    
    // Get the current state
    CHTLState* getCurrentState() const;
    
    // Reset the state machine
    void reset();
    
    // Check if there's an error
    bool isError() const;
    
    // Get error message
    std::string getErrorMessage() const;

private:
    std::unique_ptr<CHTLState> currentState_;
    bool hasError_;
    std::string errorMessage_;
};

} // namespace CHTL

#endif // CHTL_STATE_H