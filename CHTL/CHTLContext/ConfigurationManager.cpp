#include "ConfigurationManager.h"
#include <algorithm>

namespace CHTL {

void ConfigurationManager::initialize_defaults() {
    keywords["style"] = "style";
    keywords["script"] = "script";
    keywords["text"] = "text";
    keywords["if"] = "if";
    keywords["else"] = "else";
    keywords["except"] = "except";
    keywords["use"] = "use";
    keywords["html5"] = "html5";
    keywords["from"] = "from";
    // Add other keywords as needed
}

ConfigurationManager::ConfigurationManager() {
    initialize_defaults();
}

void ConfigurationManager::update(const std::shared_ptr<ConfigurationNode>& configNode) {
    if (!configNode) return;

    // Only process unnamed configurations for now as per CHTL.md
    if (!configNode->name.empty()) {
        return;
    }

    auto it = configNode->nested_blocks.find("Name");
    if (it != configNode->nested_blocks.end()) {
        const auto& name_properties = it->second;
        for (const auto& prop : name_properties) {
            // The key from the config becomes the internal, stable key for the keyword map
            // The value from the config becomes the new user-facing keyword
            std::string internal_key = prop->getKey();
            std::transform(internal_key.begin(), internal_key.end(), internal_key.begin(), ::tolower);

            // This logic is simplified. A real implementation would need to handle
            // mapping from KEYWORD_STYLE to "style" etc.
            if (keywords.count(internal_key)) {
                keywords[internal_key] = prop->getValue();
            }
        }
    }
}

std::string ConfigurationManager::getKeyword(const std::string& key) const {
    auto it = keywords.find(key);
    if (it != keywords.end()) {
        return it->second;
    }
    return ""; // Or throw an error
}

bool ConfigurationManager::isKeyword(const std::string& lexeme) const {
    for (const auto& pair : keywords) {
        if (pair.second == lexeme) {
            return true;
        }
    }
    return false;
}

}