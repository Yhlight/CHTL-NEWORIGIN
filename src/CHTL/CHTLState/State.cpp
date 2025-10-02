/**
 * @file State.cpp
 * @brief Implementation of CHTL State Machine
 */

#include "State.h"
#include "CHTL/CHTLContext.h"
#include <iostream>

namespace CHTL {

// ===== InitialState =====

std::shared_ptr<State> InitialState::handle(const Token& token, CHTLContext& /* context */) {
    if (token.type == TokenType::IDENTIFIER) {
        // Start of an element
        return std::make_shared<ElementState>();
    } else if (token.type == TokenType::KEYWORD_TEMPLATE ||
               token.type == TokenType::KEYWORD_CUSTOM) {
        return std::make_shared<TemplateState>();
    } else if (token.type == TokenType::KEYWORD_STYLE) {
        return std::make_shared<StyleBlockState>();
    } else if (token.type == TokenType::KEYWORD_SCRIPT) {
        return std::make_shared<ScriptBlockState>();
    } else if (token.type == TokenType::KEYWORD_TEXT) {
        return std::make_shared<TextBlockState>();
    }
    
    // Stay in initial state for other tokens
    return nullptr;
}

bool InitialState::isValidToken(const Token& token) const {
    return token.type == TokenType::IDENTIFIER ||
           isKeyword(token.type) ||
           token.type == TokenType::LINE_COMMENT ||
           token.type == TokenType::BLOCK_COMMENT ||
           token.type == TokenType::GENERATOR_COMMENT;
}

// ===== ElementState =====

std::shared_ptr<State> ElementState::handle(const Token& token, CHTLContext& /* context */) {
    if (token.type == TokenType::LEFT_BRACE) {
        // Element body starts - stay in element state
        return nullptr;
    } else if (token.type == TokenType::RIGHT_BRACE) {
        // Element ends - return to initial state
        return std::make_shared<InitialState>();
    } else if (token.type == TokenType::KEYWORD_STYLE) {
        return std::make_shared<StyleBlockState>();
    } else if (token.type == TokenType::KEYWORD_SCRIPT) {
        return std::make_shared<ScriptBlockState>();
    } else if (token.type == TokenType::KEYWORD_TEXT) {
        return std::make_shared<TextBlockState>();
    }
    
    return nullptr;
}

bool ElementState::isValidToken(const Token& token) const {
    return token.type == TokenType::IDENTIFIER ||
           token.type == TokenType::LEFT_BRACE ||
           token.type == TokenType::RIGHT_BRACE ||
           token.type == TokenType::COLON ||
           token.type == TokenType::SEMICOLON ||
           isKeyword(token.type) ||
           isLiteral(token.type);
}

void ElementState::onEnter(CHTLContext& context) {
    if (context.isDebugMode()) {
        std::cout << "Entering element state" << std::endl;
    }
}

// ===== StyleBlockState =====

std::shared_ptr<State> StyleBlockState::handle(const Token& token, CHTLContext& /* context */) {
    if (token.type == TokenType::RIGHT_BRACE) {
        // Style block ends
        return std::make_shared<ElementState>();
    }
    
    // Continue processing style tokens
    return nullptr;
}

bool StyleBlockState::isValidToken(const Token& token) const {
    return token.type != TokenType::EOF_TOKEN &&
           token.type != TokenType::ERROR;
}

void StyleBlockState::onEnter(CHTLContext& context) {
    if (context.isDebugMode()) {
        std::cout << "Entering style block state" << std::endl;
    }
}

// ===== ScriptBlockState =====

std::shared_ptr<State> ScriptBlockState::handle(const Token& token, CHTLContext& /* context */) {
    if (token.type == TokenType::RIGHT_BRACE) {
        // Script block ends
        return std::make_shared<ElementState>();
    }
    
    // Continue processing script tokens
    // Note: Script content should be passed to CHTL JS compiler via salt bridge
    return nullptr;
}

bool ScriptBlockState::isValidToken(const Token& token) const {
    return token.type != TokenType::EOF_TOKEN &&
           token.type != TokenType::ERROR;
}

void ScriptBlockState::onEnter(CHTLContext& context) {
    if (context.isDebugMode()) {
        std::cout << "Entering script block state" << std::endl;
    }
}

// ===== TextBlockState =====

std::shared_ptr<State> TextBlockState::handle(const Token& token, CHTLContext& /* context */) {
    if (token.type == TokenType::RIGHT_BRACE) {
        // Text block ends
        return std::make_shared<ElementState>();
    }
    
    return nullptr;
}

bool TextBlockState::isValidToken(const Token& token) const {
    return isLiteral(token.type) ||
           token.type == TokenType::LEFT_BRACE ||
           token.type == TokenType::RIGHT_BRACE;
}

// ===== TemplateState =====

std::shared_ptr<State> TemplateState::handle(const Token& token, CHTLContext& /* context */) {
    if (token.type == TokenType::RIGHT_BRACE) {
        // Template definition ends
        return std::make_shared<InitialState>();
    }
    
    return nullptr;
}

bool TemplateState::isValidToken(const Token& /* token */) const {
    return true; // Template can contain various tokens
}

// ===== StateManager =====

StateManager::StateManager()
    : currentState_(std::make_shared<InitialState>())
    , previousState_(nullptr)
{
}

void StateManager::transitionTo(std::shared_ptr<State> newState, CHTLContext& context) {
    if (newState == nullptr) {
        return; // No transition
    }
    
    if (currentState_) {
        currentState_->onExit(context);
    }
    
    previousState_ = currentState_;
    currentState_ = newState;
    
    if (currentState_) {
        currentState_->onEnter(context);
    }
}

void StateManager::handleToken(const Token& token, CHTLContext& context) {
    if (!currentState_) {
        currentState_ = std::make_shared<InitialState>();
    }
    
    if (!currentState_->isValidToken(token)) {
        if (context.isDebugMode()) {
            std::cerr << "Invalid token for current state: " << token.toString() << std::endl;
        }
        return;
    }
    
    auto nextState = currentState_->handle(token, context);
    if (nextState) {
        transitionTo(nextState, context);
    }
}

void StateManager::reset() {
    currentState_ = std::make_shared<InitialState>();
    previousState_ = nullptr;
}

} // namespace CHTL
