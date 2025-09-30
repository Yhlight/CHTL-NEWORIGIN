#include "CHTL/CHTLState.h"
#include <stdexcept>
#include <sstream>

namespace CHTL {

// InitialState Implementation
std::unique_ptr<CHTLState> InitialState::processToken(const std::string& token) {
    if (token == "html" || token == "div" || token == "span" || token == "body" || token == "head") {
        return std::make_unique<ElementState>();
    } else if (token == "text") {
        return std::make_unique<TextState>();
    } else if (token == "//" || token == "/*") {
        return std::make_unique<CommentState>();
    } else if (token == "[Template]") {
        return std::make_unique<TemplateState>();
    } else if (token == "[Custom]") {
        return std::make_unique<CustomState>();
    } else if (token == "[Origin]") {
        return std::make_unique<OriginState>();
    } else if (token == "[Import]") {
        return std::make_unique<ImportState>();
    } else if (token == "[Configuration]") {
        return std::make_unique<ConfigState>();
    } else if (token == "[Namespace]") {
        return std::make_unique<NamespaceState>();
    } else if (token == "delete" || token == "insert") {
        return std::make_unique<OperatorState>();
    }
    return std::make_unique<ErrorState>("Unexpected token: " + token);
}

// ElementState Implementation
std::unique_ptr<CHTLState> ElementState::processToken(const std::string& token) {
    if (token == "{") {
        return std::make_unique<AttributeState>();
    } else if (token == "style") {
        return std::make_unique<StyleState>();
    } else if (token == "script") {
        return std::make_unique<ScriptState>();
    } else if (token == "text") {
        return std::make_unique<TextState>();
    }
    return std::make_unique<ErrorState>("Expected '{' or element content, got: " + token);
}

// AttributeState Implementation
std::unique_ptr<CHTLState> AttributeState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    } else if (token == "style") {
        return std::make_unique<StyleState>();
    } else if (token == "script") {
        return std::make_unique<ScriptState>();
    } else if (token == "text") {
        return std::make_unique<TextState>();
    }
    // Attribute parsing logic here
    return std::make_unique<AttributeState>();
}

// TextState Implementation
std::unique_ptr<CHTLState> TextState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<TextState>();
}

// CommentState Implementation
std::unique_ptr<CHTLState> CommentState::processToken(const std::string& token) {
    if (token == "\n" || token == "*/") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<CommentState>();
}

// StyleState Implementation
std::unique_ptr<CHTLState> StyleState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<StyleState>();
}

// ScriptState Implementation
std::unique_ptr<CHTLState> ScriptState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<ScriptState>();
}

// TemplateState Implementation
std::unique_ptr<CHTLState> TemplateState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<TemplateState>();
}

// CustomState Implementation
std::unique_ptr<CHTLState> CustomState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<CustomState>();
}

// OriginState Implementation
std::unique_ptr<CHTLState> OriginState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<OriginState>();
}

// ImportState Implementation
std::unique_ptr<CHTLState> ImportState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<ImportState>();
}

// ConfigState Implementation
std::unique_ptr<CHTLState> ConfigState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<ConfigState>();
}

// NamespaceState Implementation
std::unique_ptr<CHTLState> NamespaceState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<NamespaceState>();
}

// OperatorState Implementation
std::unique_ptr<CHTLState> OperatorState::processToken(const std::string& token) {
    if (token == "}") {
        return std::make_unique<InitialState>();
    }
    return std::make_unique<OperatorState>();
}

// ErrorState Implementation
ErrorState::ErrorState(const std::string& message) : errorMessage_(message) {}

std::unique_ptr<CHTLState> ErrorState::processToken(const std::string& token) {
    return std::make_unique<ErrorState>(errorMessage_);
}

// State Factory Implementation
std::unique_ptr<CHTLState> CHTLStateFactory::createState(StateType type) {
    switch (type) {
        case StateType::INITIAL:
            return std::make_unique<InitialState>();
        case StateType::ELEMENT:
            return std::make_unique<ElementState>();
        case StateType::ATTRIBUTE:
            return std::make_unique<AttributeState>();
        case StateType::TEXT:
            return std::make_unique<TextState>();
        case StateType::COMMENT:
            return std::make_unique<CommentState>();
        case StateType::STYLE:
            return std::make_unique<StyleState>();
        case StateType::SCRIPT:
            return std::make_unique<ScriptState>();
        case StateType::TEMPLATE:
            return std::make_unique<TemplateState>();
        case StateType::CUSTOM:
            return std::make_unique<CustomState>();
        case StateType::ORIGIN:
            return std::make_unique<OriginState>();
        case StateType::IMPORT:
            return std::make_unique<ImportState>();
        case StateType::CONFIG:
            return std::make_unique<ConfigState>();
        case StateType::NAMESPACE:
            return std::make_unique<NamespaceState>();
        case StateType::OPERATOR:
            return std::make_unique<OperatorState>();
        case StateType::ERROR:
            return std::make_unique<ErrorState>("Unknown error");
        default:
            return std::make_unique<ErrorState>("Invalid state type");
    }
}

// State Manager Implementation
CHTLStateManager::CHTLStateManager() 
    : currentState_(std::make_unique<InitialState>())
    , hasError_(false) {
}

void CHTLStateManager::initialize() {
    currentState_ = std::make_unique<InitialState>();
    hasError_ = false;
    errorMessage_.clear();
}

bool CHTLStateManager::processToken(const std::string& token) {
    if (hasError_) {
        return false;
    }
    
    auto nextState = currentState_->processToken(token);
    if (nextState->getStateType() == StateType::ERROR) {
        hasError_ = true;
        errorMessage_ = static_cast<ErrorState*>(nextState.get())->getErrorMessage();
        return false;
    }
    
    currentState_ = std::move(nextState);
    return true;
}

CHTLState* CHTLStateManager::getCurrentState() const {
    return currentState_.get();
}

void CHTLStateManager::reset() {
    initialize();
}

bool CHTLStateManager::isError() const {
    return hasError_;
}

std::string CHTLStateManager::getErrorMessage() const {
    return errorMessage_;
}

} // namespace CHTL