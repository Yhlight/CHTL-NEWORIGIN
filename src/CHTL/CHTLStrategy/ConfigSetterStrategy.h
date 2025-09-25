#pragma once

#include <string>
#include <memory>
#include <stdexcept> // For std::invalid_argument
#include "../CHTLManage/ConfigurationManager.h" // For ConfigSet

// Abstract base class for configuration setting strategies.
class IConfigSetter {
public:
    virtual ~IConfigSetter() = default;
    virtual void set(ConfigSet& configSet, const std::string& value) = 0;
};

// Concrete strategy for setting the DEBUG_MODE.
class DebugModeSetter : public IConfigSetter {
public:
    void set(ConfigSet& configSet, const std::string& value) override {
        configSet.debugMode = (value == "true");
    }
};

// Concrete strategy for setting the INDEX_INITIAL_COUNT.
class IndexInitialCountSetter : public IConfigSetter {
public:
    void set(ConfigSet& configSet, const std::string& value) override {
        try {
            configSet.indexInitialCount = std::stoi(value);
        } catch (const std::invalid_argument& e) {
            // Handle error gracefully, maybe log it or throw a more specific exception
        }
    }
};

// Concrete strategy for setting DISABLE_STYLE_AUTO_ADD_CLASS.
class DisableStyleAutoAddClassSetter : public IConfigSetter {
public:
    void set(ConfigSet& configSet, const std::string& value) override {
        configSet.disableStyleAutoAddClass = (value == "true");
    }
};

// Concrete strategy for setting DISABLE_STYLE_AUTO_ADD_ID.
class DisableStyleAutoAddIdSetter : public IConfigSetter {
public:
    void set(ConfigSet& configSet, const std::string& value) override {
        configSet.disableStyleAutoAddId = (value == "true");
    }
};