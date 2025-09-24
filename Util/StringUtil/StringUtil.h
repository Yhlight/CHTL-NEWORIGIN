#pragma once

#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL::Util {

class StringUtil {
public:
    // String manipulation
    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    
    // String splitting and joining
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // String searching and replacement
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool contains(const std::string& str, const std::string& substr);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // String validation
    static bool isNumeric(const std::string& str);
    static bool isAlpha(const std::string& str);
    static bool isAlphaNumeric(const std::string& str);
    static bool isEmpty(const std::string& str);
    static bool isWhitespace(const std::string& str);
    
    // String formatting
    static std::string format(const std::string& format, const std::vector<std::string>& args);
    static std::string padLeft(const std::string& str, size_t width, char padChar = ' ');
    static std::string padRight(const std::string& str, size_t width, char padChar = ' ');
    
    // String escaping
    static std::string escapeHtml(const std::string& str);
    static std::string escapeJson(const std::string& str);
    static std::string escapeRegex(const std::string& str);
    static std::string unescapeHtml(const std::string& str);
    
    // String parsing
    static int parseInt(const std::string& str, int defaultValue = 0);
    static double parseDouble(const std::string& str, double defaultValue = 0.0);
    static bool parseBool(const std::string& str, bool defaultValue = false);
    
    // String generation
    static std::string generateId(size_t length = 8);
    static std::string generateUuid();
    static std::string repeat(const std::string& str, size_t count);
    
    // Pattern matching
    static bool matches(const std::string& str, const std::string& pattern);
    static std::vector<std::string> extractMatches(const std::string& str, const std::string& pattern);
    
    // CHTL-specific utilities
    static std::string normalizeLiteral(const std::string& str);
    static std::string escapeLiteral(const std::string& str);
    static std::string unescapeLiteral(const std::string& str);
    static bool isUnquotedLiteral(const std::string& str);
    static std::string removeQuotes(const std::string& str);
    
    // CSS/JS specific utilities
    static std::string extractCssValue(const std::string& str);
    static std::string extractJsExpression(const std::string& str);
    static bool isValidCssProperty(const std::string& str);
    static bool isValidJsIdentifier(const std::string& str);
    
private:
    static std::string charsToEscapeHtml(const char c);
    static std::string charsToEscapeJson(const char c);
};

} // namespace CHTL::Util