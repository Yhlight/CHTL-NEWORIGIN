/**
 * @file State.h
 * @brief CHTL State Machine
 * 
 * Implements the state machine for CHTL parsing
 * Following the State Pattern for managing different parsing states
 */

#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <memory>
#include <string>
#include "CHTL/CHTLLexer/Token.h"

namespace CHTL {

class CHTLContext;

/**
 * @enum ParserState
 * @brief Different states in the parsing process
 */
enum class ParserState {
    INITIAL,                // Initial state
    IN_ELEMENT,            // Inside an element definition
    IN_ATTRIBUTES,         // Parsing element attributes
    IN_STYLE_BLOCK,        // Inside a style block
    IN_SCRIPT_BLOCK,       // Inside a script block
    IN_TEXT_BLOCK,         // Inside a text block
    IN_TEMPLATE,           // Inside a template definition
    IN_CUSTOM,             // Inside a custom definition
    IN_ORIGIN,             // Inside an origin block
    IN_IMPORT,             // Processing import statement
    IN_NAMESPACE,          // Inside a namespace
    IN_CONFIG,             // Inside configuration
    IN_COMMENT,            // Processing comment
    ERROR_STATE            // Error state
};

/**
 * @class State
 * @brief Base class for all parser states
 */
class State {
public:
    virtual ~State() = default;
    
    /**
     * @brief Handle a token in current state
     * @param token The token to process
     * @param context The compilation context
     * @return Next state
     */
    virtual std::shared_ptr<State> handle(const Token& token, CHTLContext& context) = 0;
    
    /**
     * @brief Get the state type
     */
    virtual ParserState getStateType() const = 0;
    
    /**
     * @brief Enter this state
     */
    virtual void onEnter(CHTLContext& /* context */) {}
    
    /**
     * @brief Exit this state
     */
    virtual void onExit(CHTLContext& /* context */) {}
    
    /**
     * @brief Check if this is a valid token for this state
     */
    virtual bool isValidToken(const Token& token) const = 0;
};

/**
 * @class InitialState
 * @brief Initial parsing state
 */
class InitialState : public State {
public:
    std::shared_ptr<State> handle(const Token& token, CHTLContext& context) override;
    ParserState getStateType() const override { return ParserState::INITIAL; }
    bool isValidToken(const Token& token) const override;
};

/**
 * @class ElementState
 * @brief State for parsing HTML elements
 */
class ElementState : public State {
public:
    std::shared_ptr<State> handle(const Token& token, CHTLContext& context) override;
    ParserState getStateType() const override { return ParserState::IN_ELEMENT; }
    bool isValidToken(const Token& token) const override;
    void onEnter(CHTLContext& context) override;
};

/**
 * @class StyleBlockState
 * @brief State for parsing style blocks
 */
class StyleBlockState : public State {
public:
    std::shared_ptr<State> handle(const Token& token, CHTLContext& context) override;
    ParserState getStateType() const override { return ParserState::IN_STYLE_BLOCK; }
    bool isValidToken(const Token& token) const override;
    void onEnter(CHTLContext& context) override;
};

/**
 * @class ScriptBlockState
 * @brief State for parsing script blocks
 */
class ScriptBlockState : public State {
public:
    std::shared_ptr<State> handle(const Token& token, CHTLContext& context) override;
    ParserState getStateType() const override { return ParserState::IN_SCRIPT_BLOCK; }
    bool isValidToken(const Token& token) const override;
    void onEnter(CHTLContext& context) override;
};

/**
 * @class TextBlockState
 * @brief State for parsing text blocks
 */
class TextBlockState : public State {
public:
    std::shared_ptr<State> handle(const Token& token, CHTLContext& context) override;
    ParserState getStateType() const override { return ParserState::IN_TEXT_BLOCK; }
    bool isValidToken(const Token& token) const override;
};

/**
 * @class TemplateState
 * @brief State for parsing template definitions
 */
class TemplateState : public State {
public:
    std::shared_ptr<State> handle(const Token& token, CHTLContext& context) override;
    ParserState getStateType() const override { return ParserState::IN_TEMPLATE; }
    bool isValidToken(const Token& token) const override;
};

/**
 * @class StateManager
 * @brief Manages state transitions
 */
class StateManager {
public:
    StateManager();
    
    /**
     * @brief Transition to a new state
     */
    void transitionTo(std::shared_ptr<State> newState, CHTLContext& context);
    
    /**
     * @brief Handle a token in current state
     */
    void handleToken(const Token& token, CHTLContext& context);
    
    /**
     * @brief Get current state
     */
    std::shared_ptr<State> getCurrentState() const { return currentState_; }
    
    /**
     * @brief Reset to initial state
     */
    void reset();

private:
    std::shared_ptr<State> currentState_;
    std::shared_ptr<State> previousState_;
};

} // namespace CHTL

#endif // CHTL_STATE_H
