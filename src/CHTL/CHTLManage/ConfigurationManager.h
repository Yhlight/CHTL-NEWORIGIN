#pragma once

#include <string>
#include <map>
#include <vector>

// This class holds all the settings defined in a [Configuration] block.
// An instance of this manager will be held by the main Parser.
class ConfigurationManager {
public:
    // --- General Settings ---
    int indexInitialCount = 0;
    bool debugMode = false;

    // --- Feature Toggles ---
    bool disableStyleAutoAddClass = false;
    bool disableStyleAutoAddId = false;
    bool disableDefaultNamespace = false;
    bool disableCustomOriginType = false;

    // --- Keyword Customization ---
    // Maps the internal keyword name (e.g., "KEYWORD_DELETE") to the user-defined value (e.g., "remove").
    std::map<std::string, std::string> keywordMap;

    // Returns the user-defined keyword for a given internal keyword name.
    // If not customized, it returns the default value.
    std::string getKeyword(const std::string& internalName, const std::string& defaultValue) const {
        auto it = keywordMap.find(internalName);
        if (it != keywordMap.end()) {
            return it->second;
        }
        return defaultValue;
    }
};
