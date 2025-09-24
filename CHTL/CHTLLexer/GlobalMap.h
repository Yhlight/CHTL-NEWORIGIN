#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>

namespace CHTL {

// Global map for keyword and symbol resolution
class GlobalMap {
public:
    using TokenTypeMap = std::unordered_map<std::string, TokenType>;
    using KeywordMap = std::map<std::string, std::vector<std::string>>;
    
    static GlobalMap& getInstance();
    
    // Keyword management
    void registerKeyword(const std::string& name, TokenType type);
    void registerKeywordGroup(const std::string& groupName, const std::vector<std::string>& keywords);
    void unregisterKeyword(const std::string& name);
    
    // Symbol management
    void registerSymbol(const std::string& symbol, TokenType type);
    void unregisterSymbol(const std::string& symbol);
    
    // Lookup operations
    TokenType getTokenType(const std::string& identifier) const;
    bool isKeyword(const std::string& identifier) const;
    bool isSymbol(const std::string& identifier) const;
    bool isOperator(const std::string& identifier) const;
    bool isDelimiter(const std::string& identifier) const;
    
    // Configuration management (for Name configuration group)
    void loadConfiguration(const std::map<std::string, std::vector<std::string>>& config);
    void resetToDefaults();
    
    // CHTL-specific keyword groups
    void initializeCHTLKeywords();
    void initializeCHTLJSSymbols();
    
    // Utility methods
    std::vector<std::string> getKeywords() const;
    std::vector<std::string> getSymbols() const;
    std::vector<std::string> getOperators() const;
    std::vector<std::string> getDelimiters() const;
    
    // Validation
    bool isValidIdentifier(const std::string& identifier) const;
    bool isReservedKeyword(const std::string& identifier) const;
    
    // Debug and introspection
    void printKeywords() const;
    void printSymbols() const;
    std::string getDebugInfo() const;
    
private:
    GlobalMap();
    ~GlobalMap() = default;
    
    TokenTypeMap keywordMap_;
    TokenTypeMap symbolMap_;
    KeywordMap keywordGroups_;
    
    void initializeDefaults();
    void initializeOperators();
    void initializeDelimiters();
    void initializeSpecialSymbols();
};

// Configuration loader for Name group
class NameConfigurationLoader {
public:
    static std::map<std::string, std::vector<std::string>> parseNameConfiguration(const std::string& configContent);
    static void applyConfiguration(GlobalMap& globalMap, const std::map<std::string, std::vector<std::string>>& config);
    
private:
    static std::vector<std::string> parseGroupValue(const std::string& value);
    static std::string trimValue(const std::string& value);
};

// Keyword validator
class KeywordValidator {
public:
    static bool isValidKeywordName(const std::string& name);
    static bool isValidKeywordGroup(const std::vector<std::string>& keywords);
    static bool hasConflict(const std::string& keyword, const std::vector<std::string>& existing);
    
    // CHTL-specific validation
    static bool isValidCHTLKeyword(const std::string& keyword);
    static bool isValidCHTLJSKeyword(const std::string& keyword);
    static bool isValidTemplateKeyword(const std::string& keyword);
    static bool isValidCustomKeyword(const std::string& keyword);
    static bool isValidOriginKeyword(const std::string& keyword);
};

// Symbol resolver for dynamic keyword resolution
class SymbolResolver {
public:
    SymbolResolver(GlobalMap& globalMap);
    
    // Resolution strategies
    TokenType resolveIdentifier(const std::string& identifier) const;
    TokenType resolveOperator(const std::string& operatorStr) const;
    TokenType resolveDelimiter(const std::string& delimiter) const;
    TokenType resolveSpecialSymbol(const std::string& symbol) const;
    
    // Context-aware resolution
    TokenType resolveInContext(const std::string& identifier, const std::vector<Token>& context) const;
    TokenType resolveInTemplateContext(const std::string& identifier) const;
    TokenType resolveInCustomContext(const std::string& identifier) const;
    TokenType resolveInOriginContext(const std::string& identifier) const;
    
    // CJMOD-specific resolution
    TokenType resolveCHTLJSIdentifier(const std::string& identifier) const;
    TokenType resolveVirtualObject(const std::string& identifier) const;
    TokenType resolveFunctionCall(const std::string& identifier) const;
    
private:
    GlobalMap& globalMap_;
    
    bool isInTemplateContext(const std::vector<Token>& context) const;
    bool isInCustomContext(const std::vector<Token>& context) const;
    bool isInOriginContext(const std::vector<Token>& context) const;
    bool isInScriptContext(const std::vector<Token>& context) const;
};

} // namespace CHTL