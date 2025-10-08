#include "ConfigurationManager.h"

ConfigurationManager::ConfigurationManager() {
    // Initialize default keywords
    keywords["KEYWORD_DELETE"] = "delete";
    keywords["KEYWORD_INSERT"] = "insert";
    keywords["KEYWORD_AFTER"] = "after";
    keywords["KEYWORD_BEFORE"] = "before";
    keywords["KEYWORD_FROM"] = "from";
    keywords["KEYWORD_AS"] = "as";
    keywords["KEYWORD_STYLE"] = "style";
    keywords["KEYWORD_TEXT"] = "text";
    keywords["KEYWORD_TEMPLATE"] = "Template";
    keywords["KEYWORD_CUSTOM"] = "Custom";
    keywords["KEYWORD_IMPORT"] = "Import";
    keywords["KEYWORD_CONFIGURATION"] = "Configuration";
    keywords["TYPE_STYLE"] = "Style";
    keywords["TYPE_ELEMENT"] = "Element";
    keywords["TYPE_CHTL"] = "Chtl";

    // Initialize default flags
    flags["DEBUG_MODE"] = false;
    flags["DISABLE_STYLE_AUTO_ADD_CLASS"] = false;
    flags["DISABLE_STYLE_AUTO_ADD_ID"] = false;

    buildKeywordTokenMap();
}

void ConfigurationManager::buildKeywordTokenMap() {
    keywordTokenMap.clear();
    keywordTokenMap[getKeyword("KEYWORD_DELETE")] = TokenType::DeleteKeyword;
    keywordTokenMap[getKeyword("KEYWORD_INSERT")] = TokenType::InsertKeyword;
    keywordTokenMap[getKeyword("KEYWORD_AFTER")] = TokenType::AfterKeyword;
    keywordTokenMap[getKeyword("KEYWORD_BEFORE")] = TokenType::BeforeKeyword;
    keywordTokenMap[getKeyword("KEYWORD_FROM")] = TokenType::FromKeyword;
    keywordTokenMap[getKeyword("KEYWORD_AS")] = TokenType::AsKeyword;
    // Other keywords like "style", "text" etc. are handled as identifiers by the parser
}

void ConfigurationManager::setKeyword(const std::string& key, const std::string& value) {
    keywords[key] = value;
    buildKeywordTokenMap(); // Rebuild the map after a change
}

const std::map<std::string, TokenType>& ConfigurationManager::getKeywordTokenMap() {
    return keywordTokenMap;
}

std::string ConfigurationManager::getKeyword(const std::string& key) {
    if (keywords.find(key) == keywords.end()) {
        return ""; // Or throw an exception
    }
    return keywords.at(key);
}

void ConfigurationManager::setFlag(const std::string& flagName, bool value) {
    flags[flagName] = value;
}

bool ConfigurationManager::getFlag(const std::string& flagName) {
    if (flags.find(flagName) == flags.end()) {
        return false; // Default to false if flag not found
    }
    return flags.at(flagName);
}