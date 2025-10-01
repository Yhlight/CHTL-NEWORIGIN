#include "CHTLState.h"
#include "CHTLContext.h"
#include "CHTLNode.h"
#include <iostream>

namespace CHTL {

// Base CHTLState implementation
void CHTLState::logStateTransition(const std::string& from, const std::string& to) {
    // Basic logging - can be enhanced with proper logging framework
    std::cout << "State transition: " << from << " -> " << to << std::endl;
}

// InitialState implementation
std::unique_ptr<CHTLState> InitialState::handleElement(CHTLContext& context, CHTLNode* node) {
    logStateTransition("InitialState", "ParsingState");
    return std::make_unique<ParsingState>();
}

std::unique_ptr<CHTLState> InitialState::handleTemplate(CHTLContext& context, CHTLNode* node) {
    logStateTransition("InitialState", "TemplateState");
    return std::make_unique<TemplateState>();
}

std::unique_ptr<CHTLState> InitialState::handleCustom(CHTLContext& context, CHTLNode* node) {
    logStateTransition("InitialState", "ParsingState");
    return std::make_unique<ParsingState>();
}

std::unique_ptr<CHTLState> InitialState::handleImport(CHTLContext& context, CHTLNode* node) {
    logStateTransition("InitialState", "ParsingState");
    return std::make_unique<ParsingState>();
}

std::unique_ptr<CHTLState> InitialState::handleNamespace(CHTLContext& context, CHTLNode* node) {
    logStateTransition("InitialState", "ParsingState");
    return std::make_unique<ParsingState>();
}

std::unique_ptr<CHTLState> InitialState::handleConfiguration(CHTLContext& context, CHTLNode* node) {
    logStateTransition("InitialState", "ParsingState");
    return std::make_unique<ParsingState>();
}

// ParsingState implementation
std::unique_ptr<CHTLState> ParsingState::handleElement(CHTLContext& context, CHTLNode* node) {
    // Stay in parsing state for elements
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> ParsingState::handleTemplate(CHTLContext& context, CHTLNode* node) {
    logStateTransition("ParsingState", "TemplateState");
    return std::make_unique<TemplateState>();
}

std::unique_ptr<CHTLState> ParsingState::handleCustom(CHTLContext& context, CHTLNode* node) {
    // Stay in parsing state for custom elements
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> ParsingState::handleImport(CHTLContext& context, CHTLNode* node) {
    // Stay in parsing state for imports
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> ParsingState::handleNamespace(CHTLContext& context, CHTLNode* node) {
    // Stay in parsing state for namespaces
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> ParsingState::handleConfiguration(CHTLContext& context, CHTLNode* node) {
    // Stay in parsing state for configurations
    return nullptr; // No state change
}

// TemplateState implementation
std::unique_ptr<CHTLState> TemplateState::handleElement(CHTLContext& context, CHTLNode* node) {
    // Stay in template state for elements within templates
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> TemplateState::handleTemplate(CHTLContext& context, CHTLNode* node) {
    // Stay in template state for nested templates
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> TemplateState::handleCustom(CHTLContext& context, CHTLNode* node) {
    // Stay in template state for custom elements within templates
    return nullptr; // No state change
}

std::unique_ptr<CHTLState> TemplateState::handleImport(CHTLContext& context, CHTLNode* node) {
    context.addError("Import statements not allowed within templates", node ? node->getLine() : -1);
    return std::make_unique<ErrorState>();
}

std::unique_ptr<CHTLState> TemplateState::handleNamespace(CHTLContext& context, CHTLNode* node) {
    context.addError("Namespace statements not allowed within templates", node ? node->getLine() : -1);
    return std::make_unique<ErrorState>();
}

std::unique_ptr<CHTLState> TemplateState::handleConfiguration(CHTLContext& context, CHTLNode* node) {
    context.addError("Configuration statements not allowed within templates", node ? node->getLine() : -1);
    return std::make_unique<ErrorState>();
}

// ErrorState implementation
std::unique_ptr<CHTLState> ErrorState::handleElement(CHTLContext& context, CHTLNode* node) {
    context.addError("Cannot process elements in error state", node ? node->getLine() : -1);
    return nullptr; // Stay in error state
}

std::unique_ptr<CHTLState> ErrorState::handleTemplate(CHTLContext& context, CHTLNode* node) {
    context.addError("Cannot process templates in error state", node ? node->getLine() : -1);
    return nullptr; // Stay in error state
}

std::unique_ptr<CHTLState> ErrorState::handleCustom(CHTLContext& context, CHTLNode* node) {
    context.addError("Cannot process custom elements in error state", node ? node->getLine() : -1);
    return nullptr; // Stay in error state
}

std::unique_ptr<CHTLState> ErrorState::handleImport(CHTLContext& context, CHTLNode* node) {
    context.addError("Cannot process imports in error state", node ? node->getLine() : -1);
    return nullptr; // Stay in error state
}

std::unique_ptr<CHTLState> ErrorState::handleNamespace(CHTLContext& context, CHTLNode* node) {
    context.addError("Cannot process namespaces in error state", node ? node->getLine() : -1);
    return nullptr; // Stay in error state
}

std::unique_ptr<CHTLState> ErrorState::handleConfiguration(CHTLContext& context, CHTLNode* node) {
    context.addError("Cannot process configurations in error state", node ? node->getLine() : -1);
    return nullptr; // Stay in error state
}

} // namespace CHTL