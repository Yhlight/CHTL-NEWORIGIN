#pragma once

#include <memory>
#include <string>
#include <vector>

namespace CHTL {

// Forward declarations
class CHTLContext;
class CHTLNode;

// Base strategy interface following Strategy pattern
class CHTLStrategy {
public:
    virtual ~CHTLStrategy() = default;
    
    // Strategy execution methods
    virtual bool canHandle(CHTLContext& context, CHTLNode* node) = 0;
    virtual bool execute(CHTLContext& context, CHTLNode* node) = 0;
    
    // Strategy identification
    virtual std::string getStrategyName() const = 0;
    virtual int getPriority() const = 0; // Lower number = higher priority
};

// Concrete strategy implementations
class ElementParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "ElementParsingStrategy"; }
    int getPriority() const override { return 1; }
};

class TemplateParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "TemplateParsingStrategy"; }
    int getPriority() const override { return 2; }
};

class CustomParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "CustomParsingStrategy"; }
    int getPriority() const override { return 3; }
};

class ImportParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "ImportParsingStrategy"; }
    int getPriority() const override { return 4; }
};

class NamespaceParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "NamespaceParsingStrategy"; }
    int getPriority() const override { return 5; }
};

class ConfigurationParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "ConfigurationParsingStrategy"; }
    int getPriority() const override { return 6; }
};

class StyleParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "StyleParsingStrategy"; }
    int getPriority() const override { return 7; }
};

class ScriptParsingStrategy : public CHTLStrategy {
public:
    bool canHandle(CHTLContext& context, CHTLNode* node) override;
    bool execute(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStrategyName() const override { return "ScriptParsingStrategy"; }
    int getPriority() const override { return 8; }
};

// Strategy manager to coordinate strategies
class CHTLStrategyManager {
public:
    CHTLStrategyManager();
    ~CHTLStrategyManager();
    
    // Strategy registration
    void registerStrategy(std::unique_ptr<CHTLStrategy> strategy);
    
    // Strategy execution
    bool executeStrategy(CHTLContext& context, CHTLNode* node);
    
    // Strategy management
    void clearStrategies();
    size_t getStrategyCount() const;

private:
    std::vector<std::unique_ptr<CHTLStrategy>> strategies_;
    
    void sortStrategiesByPriority();
};

} // namespace CHTL