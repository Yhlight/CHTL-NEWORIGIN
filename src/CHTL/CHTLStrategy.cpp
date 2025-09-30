#include "CHTL/CHTLStrategy.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer/Token.h"

namespace CHTL {

// ElementStrategy implementation
bool ElementStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::IDENTIFIER && 
           currentState == CHTLState::StateType::INITIAL;
}

bool ElementStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement element parsing logic
    return true;
}

// AttributeStrategy implementation
bool AttributeStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::IDENTIFIER && 
           currentState == CHTLState::StateType::ELEMENT;
}

bool AttributeStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement attribute parsing logic
    return true;
}

// TextNodeStrategy implementation
bool TextNodeStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::TEXT_KEYWORD;
}

bool TextNodeStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement text node parsing logic
    return true;
}

// CommentStrategy implementation
bool CommentStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::LINE_COMMENT || 
           token.getType() == TokenType::BLOCK_COMMENT ||
           token.getType() == TokenType::GENERATOR_COMMENT;
}

bool CommentStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement comment parsing logic
    return true;
}

// StyleBlockStrategy implementation
bool StyleBlockStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::STYLE_KEYWORD;
}

bool StyleBlockStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement style block parsing logic
    return true;
}

// ScriptBlockStrategy implementation
bool ScriptBlockStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::SCRIPT_KEYWORD;
}

bool ScriptBlockStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement script block parsing logic
    return true;
}

// TemplateStrategy implementation
bool TemplateStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::TEMPLATE_KEYWORD;
}

bool TemplateStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement template parsing logic
    return true;
}

// CustomStrategy implementation
bool CustomStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::CUSTOM_KEYWORD;
}

bool CustomStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement custom parsing logic
    return true;
}

// ImportStrategy implementation
bool ImportStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::IMPORT_KEYWORD;
}

bool ImportStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement import parsing logic
    return true;
}

// NamespaceStrategy implementation
bool NamespaceStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::NAMESPACE_KEYWORD;
}

bool NamespaceStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement namespace parsing logic
    return true;
}

// ConfigurationStrategy implementation
bool ConfigurationStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::CONFIG_KEYWORD;
}

bool ConfigurationStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement configuration parsing logic
    return true;
}

// OriginBlockStrategy implementation
bool OriginBlockStrategy::canHandle(const Token& token, CHTLState::StateType currentState) const {
    return token.getType() == TokenType::ORIGIN_KEYWORD;
}

bool OriginBlockStrategy::execute(CHTLContext& context, const Token& token) {
    // TODO: Implement origin block parsing logic
    return true;
}

// CHTLStrategyManager implementation
CHTLStrategyManager::CHTLStrategyManager() = default;

void CHTLStrategyManager::registerStrategy(std::unique_ptr<CHTLStrategy> strategy) {
    if (strategy) {
        strategies_.push_back(std::move(strategy));
    }
}

CHTLStrategy* CHTLStrategyManager::findStrategy(const Token& token, CHTLState::StateType currentState) const {
    for (const auto& strategy : strategies_) {
        if (strategy->canHandle(token, currentState)) {
            return strategy.get();
        }
    }
    return nullptr;
}

bool CHTLStrategyManager::executeStrategy(CHTLContext& context, const Token& token) {
    CHTLState* currentState = context.getStateManager().getCurrentState();
    if (!currentState) {
        return false;
    }

    CHTLStrategy* strategy = findStrategy(token, currentState->getType());
    if (!strategy) {
        return false;
    }

    return strategy->execute(context, token);
}

std::vector<std::string> CHTLStrategyManager::getAvailableStrategies() const {
    std::vector<std::string> names;
    for (const auto& strategy : strategies_) {
        names.push_back(strategy->getStrategyName());
    }
    return names;
}

size_t CHTLStrategyManager::getStrategyCount() const {
    return strategies_.size();
}

} // namespace CHTL