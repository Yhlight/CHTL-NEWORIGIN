#pragma once

#include <string>
#include <map>
#include <vector>

namespace CHTL {

struct Configuration {
    // General settings
    bool DEBUG_MODE = false;
    int INDEX_INITIAL_COUNT = 0;
    bool DISABLE_DEFAULT_NAMESPACE = false;

    // Keyword Aliases from [Name] block
    // We'll store the CHTL standard name as the key, and the user-defined alias as the value.
    // This map will be pre-populated with defaults.
    std::map<std::string, std::string> keyword_map;

    Configuration() {
        // Pre-populate with default keyword names
        keyword_map["KEYWORD_TEXT"] = "text";
        keyword_map["KEYWORD_STYLE"] = "style";
        keyword_map["KEYWORD_INHERIT"] = "inherit";
        keyword_map["KEYWORD_DELETE"] = "delete";
        // ... and so on for all keywords
    }

    std::string getKeyword(const std::string& key) const {
        auto it = keyword_map.find(key);
        if (it != keyword_map.end()) {
            return it->second;
        }
        return ""; // Should not happen if all keywords are pre-populated
    }
};

} // namespace CHTL
