#include "CHTLStrategy.h"
#include "CHTLContext.h"
#include "CHTLNode.h"
#include <algorithm>

namespace CHTL {

// Strategy manager implementation
CHTLStrategyManager::CHTLStrategyManager() {
    // Register default strategies
    registerStrategy(std::make_unique<ElementParsingStrategy>());
    registerStrategy(std::make_unique<TemplateParsingStrategy>());
    registerStrategy(std::make_unique<CustomParsingStrategy>());
    registerStrategy(std::make_unique<ImportParsingStrategy>());
    registerStrategy(std::make_unique<NamespaceParsingStrategy>());
    registerStrategy(std::make_unique<ConfigurationParsingStrategy>());
    registerStrategy(std::make_unique<StyleParsingStrategy>());
    registerStrategy(std::make_unique<ScriptParsingStrategy>());
    
    sortStrategiesByPriority();
}

CHTLStrategyManager::~CHTLStrategyManager() {
}

void CHTLStrategyManager::registerStrategy(std::unique_ptr<CHTLStrategy> strategy) {
    strategies_.push_back(std::move(strategy));
}

bool CHTLStrategyManager::executeStrategy(CHTLContext& context, CHTLNode* node) {
    for (const auto& strategy : strategies_) {
        if (strategy->canHandle(context, node)) {
            return strategy->execute(context, node);
        }
    }
    
    // No strategy found
    context.addError("No strategy found to handle node type", node ? node->getLine() : -1);
    return false;
}

void CHTLStrategyManager::clearStrategies() {
    strategies_.clear();
}

size_t CHTLStrategyManager::getStrategyCount() const {
    return strategies_.size();
}

void CHTLStrategyManager::sortStrategiesByPriority() {
    std::sort(strategies_.begin(), strategies_.end(),
        [](const std::unique_ptr<CHTLStrategy>& a, const std::unique_ptr<CHTLStrategy>& b) {
            return a->getPriority() < b->getPriority();
        });
}

// ElementParsingStrategy implementation
bool ElementParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::ELEMENT;
}

bool ElementParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic element parsing logic
    // TODO: Implement actual element parsing based on CHTL.md specification
    return true;
}

// TemplateParsingStrategy implementation
bool TemplateParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::TEMPLATE;
}

bool TemplateParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic template parsing logic
    // TODO: Implement actual template parsing based on CHTL.md specification
    return true;
}

// CustomParsingStrategy implementation
bool CustomParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::CUSTOM;
}

bool CustomParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic custom parsing logic
    // TODO: Implement actual custom parsing based on CHTL.md specification
    return true;
}

// ImportParsingStrategy implementation
bool ImportParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::IMPORT;
}

bool ImportParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic import parsing logic
    // TODO: Implement actual import parsing based on CHTL.md specification
    return true;
}

// NamespaceParsingStrategy implementation
bool NamespaceParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::NAMESPACE;
}

bool NamespaceParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic namespace parsing logic
    // TODO: Implement actual namespace parsing based on CHTL.md specification
    return true;
}

// ConfigurationParsingStrategy implementation
bool ConfigurationParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::CONFIG;
}

bool ConfigurationParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic configuration parsing logic
    // TODO: Implement actual configuration parsing based on CHTL.md specification
    return true;
}

// StyleParsingStrategy implementation
bool StyleParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::STYLE;
}

bool StyleParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic style parsing logic
    // TODO: Implement actual style parsing based on CHTL.md specification
    return true;
}

// ScriptParsingStrategy implementation
bool ScriptParsingStrategy::canHandle(CHTLContext& context, CHTLNode* node) {
    return node && node->getType() == CHTLNode::Type::SCRIPT;
}

bool ScriptParsingStrategy::execute(CHTLContext& context, CHTLNode* node) {
    if (!canHandle(context, node)) {
        return false;
    }
    
    // Basic script parsing logic
    // TODO: Implement actual script parsing based on CHTL.md specification
    return true;
}

} // namespace CHTL