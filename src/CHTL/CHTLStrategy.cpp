#include "CHTL/CHTLStrategy.h"
#include <stdexcept>
#include <sstream>

namespace CHTL {

// Strategy implementations
class ElementStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Element parsing logic
        result_ = "Element processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Element";
    }
    
    bool canHandle(const std::string& input) const override {
        // Check if input is an element
        return input.find("<") != std::string::npos || 
               input == "html" || input == "div" || input == "span" || 
               input == "body" || input == "head";
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class StyleStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Style parsing logic
        result_ = "Style processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Style";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("style") != std::string::npos ||
               input.find("color") != std::string::npos ||
               input.find("width") != std::string::npos ||
               input.find("height") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class ScriptStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Script parsing logic
        result_ = "Script processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Script";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("script") != std::string::npos ||
               input.find("function") != std::string::npos ||
               input.find("var") != std::string::npos ||
               input.find("let") != std::string::npos ||
               input.find("const") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class TemplateStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Template parsing logic
        result_ = "Template processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Template";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("[Template]") != std::string::npos ||
               input.find("@Style") != std::string::npos ||
               input.find("@Element") != std::string::npos ||
               input.find("@Var") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class CustomStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Custom parsing logic
        result_ = "Custom processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Custom";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("[Custom]") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class OriginStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Origin parsing logic
        result_ = "Origin processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Origin";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("[Origin]") != std::string::npos ||
               input.find("@Html") != std::string::npos ||
               input.find("@Style") != std::string::npos ||
               input.find("@JavaScript") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class ImportStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Import parsing logic
        result_ = "Import processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Import";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("[Import]") != std::string::npos ||
               input.find("from") != std::string::npos ||
               input.find("as") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class ConfigStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Config parsing logic
        result_ = "Config processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Config";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("[Configuration]") != std::string::npos ||
               input.find("DEBUG_MODE") != std::string::npos ||
               input.find("INDEX_INITIAL_COUNT") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class NamespaceStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Namespace parsing logic
        result_ = "Namespace processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Namespace";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("[Namespace]") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

class OperatorStrategy : public CHTLStrategy {
private:
    std::string result_;
    
public:
    void initialize() override {
        result_.clear();
    }
    
    bool process(const std::string& input) override {
        // Operator parsing logic
        result_ = "Operator processed: " + input;
        return true;
    }
    
    std::string getStrategyName() const override {
        return "Operator";
    }
    
    bool canHandle(const std::string& input) const override {
        return input.find("delete") != std::string::npos ||
               input.find("insert") != std::string::npos ||
               input.find("after") != std::string::npos ||
               input.find("before") != std::string::npos ||
               input.find("replace") != std::string::npos;
    }
    
    std::string getResult() const override {
        return result_;
    }
    
    void reset() override {
        result_.clear();
    }
};

// Strategy Factory Implementation
std::unique_ptr<CHTLStrategy> CHTLStrategyFactory::createStrategy(StrategyType type) {
    switch (type) {
        case StrategyType::ELEMENT:
            return std::make_unique<ElementStrategy>();
        case StrategyType::STYLE:
            return std::make_unique<StyleStrategy>();
        case StrategyType::SCRIPT:
            return std::make_unique<ScriptStrategy>();
        case StrategyType::TEMPLATE:
            return std::make_unique<TemplateStrategy>();
        case StrategyType::CUSTOM:
            return std::make_unique<CustomStrategy>();
        case StrategyType::ORIGIN:
            return std::make_unique<OriginStrategy>();
        case StrategyType::IMPORT:
            return std::make_unique<ImportStrategy>();
        case StrategyType::CONFIG:
            return std::make_unique<ConfigStrategy>();
        case StrategyType::NAMESPACE:
            return std::make_unique<NamespaceStrategy>();
        case StrategyType::OPERATOR:
            return std::make_unique<OperatorStrategy>();
        default:
            throw std::invalid_argument("Invalid strategy type");
    }
}

// Strategy Manager Implementation
CHTLStrategyManager::CHTLStrategyManager() : currentStrategy_(nullptr) {
}

void CHTLStrategyManager::initialize() {
    // Initialize all strategies
    strategies_[CHTLStrategyFactory::StrategyType::ELEMENT] = std::make_unique<ElementStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::STYLE] = std::make_unique<StyleStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::SCRIPT] = std::make_unique<ScriptStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::TEMPLATE] = std::make_unique<TemplateStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::CUSTOM] = std::make_unique<CustomStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::ORIGIN] = std::make_unique<OriginStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::IMPORT] = std::make_unique<ImportStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::CONFIG] = std::make_unique<ConfigStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::NAMESPACE] = std::make_unique<NamespaceStrategy>();
    strategies_[CHTLStrategyFactory::StrategyType::OPERATOR] = std::make_unique<OperatorStrategy>();
    
    // Initialize all strategies
    for (auto& strategy : strategies_) {
        strategy.second->initialize();
    }
    
    currentStrategy_ = nullptr;
    results_.clear();
}

bool CHTLStrategyManager::process(const std::string& input) {
    // Find appropriate strategy
    for (auto& strategy : strategies_) {
        if (strategy.second->canHandle(input)) {
            currentStrategy_ = strategy.second.get();
            bool success = currentStrategy_->process(input);
            if (success) {
                results_.push_back(currentStrategy_->getResult());
            }
            return success;
        }
    }
    
    // No strategy found
    return false;
}

CHTLStrategy* CHTLStrategyManager::getCurrentStrategy() const {
    return currentStrategy_;
}

bool CHTLStrategyManager::setStrategy(CHTLStrategyFactory::StrategyType type) {
    auto it = strategies_.find(type);
    if (it != strategies_.end()) {
        currentStrategy_ = it->second.get();
        return true;
    }
    return false;
}

std::string CHTLStrategyManager::getAllResults() const {
    std::stringstream ss;
    for (const auto& result : results_) {
        ss << result << "\n";
    }
    return ss.str();
}

void CHTLStrategyManager::reset() {
    for (auto& strategy : strategies_) {
        strategy.second->reset();
    }
    currentStrategy_ = nullptr;
    results_.clear();
}

} // namespace CHTL