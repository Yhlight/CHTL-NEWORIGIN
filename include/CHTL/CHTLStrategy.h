#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace CHTL {

/**
 * @brief Strategy pattern interface for different parsing strategies
 * 
 * Each strategy handles a specific aspect of CHTL parsing:
 * - Element parsing strategy
 * - Style parsing strategy  
 * - Script parsing strategy
 * - Template parsing strategy
 * - Custom parsing strategy
 * - Import parsing strategy
 * - Configuration parsing strategy
 */
class CHTLStrategy {
public:
    virtual ~CHTLStrategy() = default;
    
    /**
     * @brief Initialize strategy
     */
    virtual void initialize() = 0;
    
    /**
     * @brief Process input with this strategy
     * @param input Input to process
     * @return True if successful, false if error
     */
    virtual bool process(const std::string& input) = 0;
    
    /**
     * @brief Get strategy name
     * @return Strategy name
     */
    virtual std::string getStrategyName() const = 0;
    
    /**
     * @brief Check if strategy can handle input
     * @param input Input to check
     * @return True if strategy can handle input
     */
    virtual bool canHandle(const std::string& input) const = 0;
    
    /**
     * @brief Get result of processing
     * @return Processing result
     */
    virtual std::string getResult() const = 0;
    
    /**
     * @brief Reset strategy state
     */
    virtual void reset() = 0;

protected:
    CHTLStrategy() = default;
};

/**
 * @brief Strategy factory for creating strategies
 */
class CHTLStrategyFactory {
public:
    enum class StrategyType {
        ELEMENT,
        STYLE,
        SCRIPT,
        TEMPLATE,
        CUSTOM,
        ORIGIN,
        IMPORT,
        CONFIG,
        NAMESPACE,
        OPERATOR
    };
    
    static std::unique_ptr<CHTLStrategy> createStrategy(StrategyType type);
    static void registerStrategy(StrategyType type, std::function<std::unique_ptr<CHTLStrategy>()> creator);
};

/**
 * @brief Strategy manager for coordinating different parsing strategies
 */
class CHTLStrategyManager {
public:
    CHTLStrategyManager();
    ~CHTLStrategyManager() = default;
    
    /**
     * @brief Initialize strategy manager
     */
    void initialize();
    
    /**
     * @brief Process input with appropriate strategy
     * @param input Input to process
     * @return True if successful, false if error
     */
    bool process(const std::string& input);
    
    /**
     * @brief Get current strategy
     * @return Current strategy
     */
    CHTLStrategy* getCurrentStrategy() const;
    
    /**
     * @brief Set strategy by type
     * @param type Strategy type
     * @return True if successful, false if error
     */
    bool setStrategy(CHTLStrategyFactory::StrategyType type);
    
    /**
     * @brief Get all results from strategies
     * @return Combined results
     */
    std::string getAllResults() const;
    
    /**
     * @brief Reset all strategies
     */
    void reset();

private:
    std::unordered_map<CHTLStrategyFactory::StrategyType, std::unique_ptr<CHTLStrategy>> strategies_;
    CHTLStrategy* currentStrategy_;
    std::vector<std::string> results_;
};

} // namespace CHTL