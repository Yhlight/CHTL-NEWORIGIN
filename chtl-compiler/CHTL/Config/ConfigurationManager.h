#pragma once

#include <string>
#include <map>
#include <vector>
#include "../CHTLLexer/Token.h" // For TokenType

class ConfigurationManager {
public:
    // Singleton access
    static ConfigurationManager& getInstance() {
        static ConfigurationManager instance;
        return instance;
    }

    // Keyword management
    void setKeyword(const std::string& key, const std::string& value);
    std::string getKeyword(const std::string& key);
    const std::map<std::string, TokenType>& getKeywordTokenMap();

    // Flag management
    void setFlag(const std::string& flagName, bool value);
    bool getFlag(const std::string& flagName);

private:
    // Private constructor for singleton
    ConfigurationManager();
    ~ConfigurationManager() = default;

    // Delete copy and move constructors
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) = delete;
    ConfigurationManager& operator=(ConfigurationManager&&) = delete;

    void buildKeywordTokenMap();

    std::map<std::string, std::string> keywords;
    std::map<std::string, bool> flags;
    std::map<std::string, TokenType> keywordTokenMap;
};