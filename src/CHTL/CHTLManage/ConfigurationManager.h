#pragma once

#include <string>
#include <map>
#include <vector>
#include <algorithm> // For std::find
#include <memory>

// A struct to hold all the settings for a single configuration.
struct ConfigSet {
    // --- General Settings ---
    int indexInitialCount = 0;
    bool debugMode = false;

    // --- Feature Toggles ---
    bool disableStyleAutoAddClass = false;
    bool disableStyleAutoAddId = false;
    bool disableDefaultNamespace = false;
    bool disableCustomOriginType = false;

    // --- Origin Type Whitelist ---
    // If not empty, only these custom types are allowed for [Origin] blocks.
    std::vector<std::string> allowedOriginTypes;

    // --- Keyword Customization ---
    std::map<std::string, std::vector<std::string>> keywordMap;
};

// This class manages multiple named configurations and tracks the active one.
class ConfigurationManager {
public:
    ConfigurationManager() {
        // Ensure there is always a default configuration.
        configurations["_default"] = std::make_unique<ConfigSet>();
        activeConfigName = "_default";
    }

    // Stores a new named configuration.
    void addConfig(const std::string& name, std::unique_ptr<ConfigSet> config) {
        configurations[name] = std::move(config);
    }

    // Switches the active configuration.
    void setActiveConfig(const std::string& name) {
        if (configurations.find(name) != configurations.end()) {
            activeConfigName = name;
        } else {
            // Or throw an error, depending on desired behavior
        }
    }

    // Gets a pointer to the currently active configuration set.
    ConfigSet* getActiveConfig() {
        return configurations[activeConfigName].get();
    }

    // Gets the list of aliases for a keyword from the active config.
    const std::vector<std::string>& getKeywordAliases(const std::string& internalName) const {
        static const std::vector<std::string> empty_vector;
        const auto& activeConfig = configurations.at(activeConfigName);
        auto it = activeConfig->keywordMap.find(internalName);
        if (it != activeConfig->keywordMap.end()) {
            return it->second;
        }
        return empty_vector;
    }

    // Gets the primary keyword value from the active config.
    std::string getKeyword(const std::string& internalName, const std::string& defaultValue) const {
        const auto& activeConfig = configurations.at(activeConfigName);
        auto it = activeConfig->keywordMap.find(internalName);
        if (it != activeConfig->keywordMap.end() && !it->second.empty()) {
            return it->second.front();
        }
        return defaultValue;
    }

    // Checks if a given token value matches a keyword in the active config.
    bool isKeyword(const std::string& value, const std::string& internalName, const std::string& defaultValue) const {
        const auto& activeConfig = configurations.at(activeConfigName);
        auto it = activeConfig->keywordMap.find(internalName);
        if (it != activeConfig->keywordMap.end() && !it->second.empty()) {
            const auto& aliases = it->second;
            return std::find(aliases.begin(), aliases.end(), value) != aliases.end();
        }
        return value == defaultValue;
    }

private:
    std::map<std::string, std::unique_ptr<ConfigSet>> configurations;
    std::string activeConfigName;
};