#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

// Base strategy interface
template<typename InputType, typename OutputType>
class Strategy {
public:
    virtual ~Strategy() = default;
    virtual OutputType execute(const InputType& input) = 0;
    virtual std::string getName() const = 0;
    virtual bool canHandle(const InputType& input) const { return true; }
};

// Strategy manager for dependency injection
template<typename InputType, typename OutputType>
class StrategyManager {
public:
    using StrategyPtr = std::unique_ptr<Strategy<InputType, OutputType>>;
    using StrategyFactory = std::function<StrategyPtr()>;
    
    void registerStrategy(const std::string& name, StrategyFactory factory);
    void unregisterStrategy(const std::string& name);
    Strategy<InputType, OutputType>* getStrategy(const std::string& name);
    Strategy<InputType, OutputType>* selectStrategy(const InputType& input);
    
    std::vector<std::string> getAvailableStrategies() const;
    bool hasStrategy(const std::string& name) const;
    
    void setDefaultStrategy(const std::string& name);
    std::string getDefaultStrategy() const;
    
private:
    std::map<std::string, StrategyFactory> factories_;
    std::map<std::string, StrategyPtr> instances_;
    std::string defaultStrategy_;
};

// Context-aware strategy
template<typename ContextType, typename InputType, typename OutputType>
class ContextualStrategy {
public:
    virtual ~ContextualStrategy() = default;
    virtual OutputType execute(const ContextType& context, const InputType& input) = 0;
    virtual std::string getName() const = 0;
    virtual bool canHandle(const ContextType& context, const InputType& input) const { return true; }
};

// Context-aware strategy manager
template<typename ContextType, typename InputType, typename OutputType>
class ContextualStrategyManager {
public:
    using StrategyPtr = std::unique_ptr<ContextualStrategy<ContextType, InputType, OutputType>>;
    using StrategyFactory = std::function<StrategyPtr()>;
    
    void registerStrategy(const std::string& name, StrategyFactory factory);
    void unregisterStrategy(const std::string& name);
    ContextualStrategy<ContextType, InputType, OutputType>* getStrategy(const std::string& name);
    ContextualStrategy<ContextType, InputType, OutputType>* selectStrategy(const ContextType& context, const InputType& input);
    
    std::vector<std::string> getAvailableStrategies() const;
    bool hasStrategy(const std::string& name) const;
    
    void setDefaultStrategy(const std::string& name);
    std::string getDefaultStrategy() const;
    
private:
    std::map<std::string, StrategyFactory> factories_;
    std::map<std::string, StrategyPtr> instances_;
    std::string defaultStrategy_;
};

// Template implementations for StrategyManager
template<typename InputType, typename OutputType>
void StrategyManager<InputType, OutputType>::registerStrategy(const std::string& name, StrategyFactory factory) {
    factories_[name] = std::move(factory);
}

template<typename InputType, typename OutputType>
void StrategyManager<InputType, OutputType>::unregisterStrategy(const std::string& name) {
    factories_.erase(name);
    instances_.erase(name);
}

template<typename InputType, typename OutputType>
Strategy<InputType, OutputType>* StrategyManager<InputType, OutputType>::getStrategy(const std::string& name) {
    auto factoryIt = factories_.find(name);
    if (factoryIt == factories_.end()) {
        return nullptr;
    }
    
    auto instanceIt = instances_.find(name);
    if (instanceIt == instances_.end()) {
        instances_[name] = factoryIt->second();
    }
    
    return instances_[name].get();
}

template<typename InputType, typename OutputType>
Strategy<InputType, OutputType>* StrategyManager<InputType, OutputType>::selectStrategy(const InputType& input) {
    // First try to find a strategy that can handle the input
    for (const auto& [name, factory] : factories_) {
        auto* strategy = getStrategy(name);
        if (strategy && strategy->canHandle(input)) {
            return strategy;
        }
    }
    
    // Fall back to default strategy
    if (!defaultStrategy_.empty()) {
        return getStrategy(defaultStrategy_);
    }
    
    return nullptr;
}

template<typename InputType, typename OutputType>
std::vector<std::string> StrategyManager<InputType, OutputType>::getAvailableStrategies() const {
    std::vector<std::string> names;
    for (const auto& [name, factory] : factories_) {
        names.push_back(name);
    }
    return names;
}

template<typename InputType, typename OutputType>
bool StrategyManager<InputType, OutputType>::hasStrategy(const std::string& name) const {
    return factories_.find(name) != factories_.end();
}

template<typename InputType, typename OutputType>
void StrategyManager<InputType, OutputType>::setDefaultStrategy(const std::string& name) {
    if (hasStrategy(name)) {
        defaultStrategy_ = name;
    }
}

template<typename InputType, typename OutputType>
std::string StrategyManager<InputType, OutputType>::getDefaultStrategy() const {
    return defaultStrategy_;
}

// Template implementations for ContextualStrategyManager
template<typename ContextType, typename InputType, typename OutputType>
void ContextualStrategyManager<ContextType, InputType, OutputType>::registerStrategy(const std::string& name, StrategyFactory factory) {
    factories_[name] = std::move(factory);
}

template<typename ContextType, typename InputType, typename OutputType>
void ContextualStrategyManager<ContextType, InputType, OutputType>::unregisterStrategy(const std::string& name) {
    factories_.erase(name);
    instances_.erase(name);
}

template<typename ContextType, typename InputType, typename OutputType>
ContextualStrategy<ContextType, InputType, OutputType>* ContextualStrategyManager<ContextType, InputType, OutputType>::getStrategy(const std::string& name) {
    auto factoryIt = factories_.find(name);
    if (factoryIt == factories_.end()) {
        return nullptr;
    }
    
    auto instanceIt = instances_.find(name);
    if (instanceIt == instances_.end()) {
        instances_[name] = factoryIt->second();
    }
    
    return instances_[name].get();
}

template<typename ContextType, typename InputType, typename OutputType>
ContextualStrategy<ContextType, InputType, OutputType>* ContextualStrategyManager<ContextType, InputType, OutputType>::selectStrategy(const ContextType& context, const InputType& input) {
    // First try to find a strategy that can handle the input
    for (const auto& [name, factory] : factories_) {
        auto* strategy = getStrategy(name);
        if (strategy && strategy->canHandle(context, input)) {
            return strategy;
        }
    }
    
    // Fall back to default strategy
    if (!defaultStrategy_.empty()) {
        return getStrategy(defaultStrategy_);
    }
    
    return nullptr;
}

template<typename ContextType, typename InputType, typename OutputType>
std::vector<std::string> ContextualStrategyManager<ContextType, InputType, OutputType>::getAvailableStrategies() const {
    std::vector<std::string> names;
    for (const auto& [name, factory] : factories_) {
        names.push_back(name);
    }
    return names;
}

template<typename ContextType, typename InputType, typename OutputType>
bool ContextualStrategyManager<ContextType, InputType, OutputType>::hasStrategy(const std::string& name) const {
    return factories_.find(name) != factories_.end();
}

template<typename ContextType, typename InputType, typename OutputType>
void ContextualStrategyManager<ContextType, InputType, OutputType>::setDefaultStrategy(const std::string& name) {
    if (hasStrategy(name)) {
        defaultStrategy_ = name;
    }
}

template<typename ContextType, typename InputType, typename OutputType>
std::string ContextualStrategyManager<ContextType, InputType, OutputType>::getDefaultStrategy() const {
    return defaultStrategy_;
}

} // namespace CHTL