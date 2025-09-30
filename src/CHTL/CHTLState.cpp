#include "CHTL/CHTLState.h"
#include <stdexcept>
#include <algorithm>

namespace CHTL {

// CHTLState implementation
CHTLState::CHTLState(StateType type, const std::string& context)
    : type_(type), context_(context) {
}

bool CHTLState::canTransitionTo(StateType newState) const {
    return CHTLStateFactory::isValidTransition(type_, newState);
}

std::unique_ptr<CHTLState> CHTLState::transitionTo(StateType newState, const std::string& context) const {
    return CHTLStateFactory::createState(newState, context);
}

void CHTLState::enter() {
    // Default implementation - can be overridden by derived classes
}

void CHTLState::exit() {
    // Default implementation - can be overridden by derived classes
}

// CHTLStateFactory implementation
std::unique_ptr<CHTLState> CHTLStateFactory::createState(CHTLState::StateType type, const std::string& context) {
    // This will be implemented with concrete state classes
    // For now, return a basic state
    return std::make_unique<CHTLState>(type, context);
}

bool CHTLStateFactory::isValidTransition(CHTLState::StateType from, CHTLState::StateType to) {
    // Define valid state transitions
    static const std::unordered_map<CHTLState::StateType, std::vector<CHTLState::StateType>> transitions = {
        {CHTLState::StateType::INITIAL, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT,
            CHTLState::StateType::TEMPLATE_DEFINITION,
            CHTLState::StateType::CUSTOM_DEFINITION,
            CHTLState::StateType::IMPORT_STATEMENT,
            CHTLState::StateType::NAMESPACE_DEFINITION,
            CHTLState::StateType::CONFIGURATION,
            CHTLState::StateType::ORIGIN_BLOCK
        }},
        {CHTLState::StateType::ELEMENT, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::ATTRIBUTE,
            CHTLState::StateType::TEXT_NODE,
            CHTLState::StateType::COMMENT,
            CHTLState::StateType::STYLE_BLOCK,
            CHTLState::StateType::SCRIPT_BLOCK,
            CHTLState::StateType::ORIGIN_BLOCK
        }},
        {CHTLState::StateType::ATTRIBUTE, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::ATTRIBUTE,
            CHTLState::StateType::TEXT_NODE,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::TEXT_NODE, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::COMMENT, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::TEXT_NODE,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::STYLE_BLOCK, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::SCRIPT_BLOCK, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::TEMPLATE_DEFINITION, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::CUSTOM_DEFINITION, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::IMPORT_STATEMENT, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::NAMESPACE_DEFINITION, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::CONFIGURATION, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }},
        {CHTLState::StateType::ORIGIN_BLOCK, {
            CHTLState::StateType::ELEMENT,
            CHTLState::StateType::COMMENT
        }}
    };

    auto it = transitions.find(from);
    if (it == transitions.end()) {
        return false;
    }

    return std::find(it->second.begin(), it->second.end(), to) != it->second.end();
}

// CHTLStateManager implementation
CHTLStateManager::CHTLStateManager() {
    // Start with initial state
    pushState(std::make_unique<CHTLState>(CHTLState::StateType::INITIAL));
}

void CHTLStateManager::pushState(std::unique_ptr<CHTLState> state) {
    if (state) {
        stateStack_.push_back(std::move(state));
    }
}

void CHTLStateManager::popState() {
    if (!stateStack_.empty()) {
        stateStack_.pop_back();
    }
}

CHTLState* CHTLStateManager::getCurrentState() const {
    if (stateStack_.empty()) {
        return nullptr;
    }
    return stateStack_.back().get();
}

CHTLState* CHTLStateManager::getPreviousState() const {
    if (stateStack_.size() < 2) {
        return nullptr;
    }
    return stateStack_[stateStack_.size() - 2].get();
}

bool CHTLStateManager::transitionTo(CHTLState::StateType newState, const std::string& context) {
    CHTLState* current = getCurrentState();
    if (!current) {
        return false;
    }

    if (!CHTLStateFactory::isValidTransition(current->getType(), newState)) {
        setErrorState("Invalid state transition from " + std::to_string(static_cast<int>(current->getType())) + 
                     " to " + std::to_string(static_cast<int>(newState)));
        return false;
    }

    auto newStatePtr = CHTLStateFactory::createState(newState, context);
    if (newStatePtr) {
        pushState(std::move(newStatePtr));
        return true;
    }

    return false;
}

void CHTLStateManager::resetToInitial() {
    stateStack_.clear();
    pushState(std::make_unique<CHTLState>(CHTLState::StateType::INITIAL));
    errorMessage_.clear();
}

size_t CHTLStateManager::getStateStackSize() const {
    return stateStack_.size();
}

bool CHTLStateManager::isInState(CHTLState::StateType stateType) const {
    CHTLState* current = getCurrentState();
    return current && current->getType() == stateType;
}

void CHTLStateManager::setErrorState(const std::string& errorMessage) {
    errorMessage_ = errorMessage;
    pushState(std::make_unique<CHTLState>(CHTLState::StateType::ERROR, errorMessage));
}

bool CHTLStateManager::hasError() const {
    return !errorMessage_.empty() || (getCurrentState() && getCurrentState()->isError());
}

const std::string& CHTLStateManager::getErrorMessage() const {
    return errorMessage_;
}

} // namespace CHTL