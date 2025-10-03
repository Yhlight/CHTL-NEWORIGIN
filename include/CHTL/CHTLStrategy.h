#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "CHTL/CHTLState.h"

namespace CHTL {

// Forward declarations
class CHTLContext;
class Token;

/**
 * @brief Strategy interface for different parsing strategies
 * 
 * The strategy pattern allows different parsing approaches for different
 * syntax elements (elements, styles, scripts, templates, etc.)
 */
class CHTLStrategy {
public:
    virtual ~CHTLStrategy() = default;

    // Strategy execution
    virtual bool canHandle(const Token& token, CHTLState::StateType currentState) const = 0;
    virtual bool execute(CHTLContext& context, const Token& token) = 0;
    virtual std::string getStrategyName() const = 0;

    // Strategy lifecycle
    virtual void initialize() {}
    virtual void cleanup() {}
};

/**
 * @brief Element parsing strategy
 */
class ElementStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "ElementStrategy"; }
};

/**
 * @brief Attribute parsing strategy
 */
class AttributeStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "AttributeStrategy"; }
};

/**
 * @brief Text node parsing strategy
 */
class TextNodeStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "TextNodeStrategy"; }
};

/**
 * @brief Comment parsing strategy
 */
class CommentStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "CommentStrategy"; }
};

/**
 * @brief Style block parsing strategy
 */
class StyleBlockStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "StyleBlockStrategy"; }
};

/**
 * @brief Script block parsing strategy
 */
class ScriptBlockStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "ScriptBlockStrategy"; }
};

/**
 * @brief Template definition parsing strategy
 */
class TemplateStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "TemplateStrategy"; }
};

/**
 * @brief Custom definition parsing strategy
 */
class CustomStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "CustomStrategy"; }
};

/**
 * @brief Import statement parsing strategy
 */
class ImportStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "ImportStrategy"; }
};

/**
 * @brief Namespace definition parsing strategy
 */
class NamespaceStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "NamespaceStrategy"; }
};

/**
 * @brief Configuration parsing strategy
 */
class ConfigurationStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "ConfigurationStrategy"; }
};

/**
 * @brief Origin block parsing strategy
 */
class OriginBlockStrategy : public CHTLStrategy {
public:
    bool canHandle(const Token& token, CHTLState::StateType currentState) const override;
    bool execute(CHTLContext& context, const Token& token) override;
    std::string getStrategyName() const override { return "OriginBlockStrategy"; }
};

/**
 * @brief Strategy manager for coordinating different parsing strategies
 */
class CHTLStrategyManager {
public:
    CHTLStrategyManager();
    ~CHTLStrategyManager() = default;

    // Strategy management
    void registerStrategy(std::unique_ptr<CHTLStrategy> strategy);
    CHTLStrategy* findStrategy(const Token& token, CHTLState::StateType currentState) const;
    
    // Strategy execution
    bool executeStrategy(CHTLContext& context, const Token& token);
    
    // Strategy information
    std::vector<std::string> getAvailableStrategies() const;
    size_t getStrategyCount() const;

private:
    std::vector<std::unique_ptr<CHTLStrategy>> strategies_;
};

} // namespace CHTL