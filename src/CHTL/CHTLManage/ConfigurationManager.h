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
    // Maps internal keyword names (e.g., "KEYWORD_DELETE") to a list of user-defined aliases.
    std::map<std::string, std::vector<std::string>> keywordMap;

    // Gets the list of aliases for a keyword. Returns an empty vector if not defined.
    const std::vector<std::string>& getKeywordAliases(const std::string& internalName) const {
        static const std::vector<std::string> empty_vector;
        auto it = keywordMap.find(internalName);
        if (it != keywordMap.end()) {
            return it->second;
        }
        return empty_vector;
    }

    // Gets the primary keyword value (the first in the list) or a default.
    std::string getKeyword(const std::string& internalName, const std::string& defaultValue) const {
        auto it = keywordMap.find(internalName);
        if (it != keywordMap.end() && !it->second.empty()) {
            return it->second.front();
        }
        return defaultValue;
    }
};
