#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * @brief Global map for managing global symbols and identifiers
 * 
 * This class manages global symbols, keywords, and identifiers
 * used throughout the CHTL compilation process.
 */
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap() = default;

    // Symbol management
    void addSymbol(const std::string& name, const std::string& value);
    std::string getSymbol(const std::string& name) const;
    bool hasSymbol(const std::string& name) const;
    void removeSymbol(const std::string& name);

    // Keyword management
    void addKeyword(const std::string& keyword);
    bool isKeyword(const std::string& word) const;

    // Identifier management
    void addIdentifier(const std::string& identifier);
    bool isIdentifier(const std::string& word) const;

    // Clear all data
    void clear();

    // Get statistics
    size_t getSymbolCount() const;
    size_t getKeywordCount() const;
    size_t getIdentifierCount() const;

private:
    std::unordered_map<std::string, std::string> symbols_;
    std::unordered_map<std::string, bool> keywords_;
    std::unordered_map<std::string, bool> identifiers_;
};

} // namespace CHTL