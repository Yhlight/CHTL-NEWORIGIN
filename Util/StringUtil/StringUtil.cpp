#include "StringUtil.h"
#include <random>
#include <iomanip>
#include <sstream>

namespace CHTL::Util {

std::string StringUtil::trim(const std::string& str) {
    return trimLeft(trimRight(str));
}

std::string StringUtil::trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtil::trimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string StringUtil::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtil::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

std::string StringUtil::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    if (parts.empty()) return "";
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += delimiter + parts[i];
    }
    
    return result;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

bool StringUtil::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string StringUtil::replace(const std::string& str, const std::string& from, const std::string& to) {
    size_t pos = str.find(from);
    if (pos == std::string::npos) return str;
    
    std::string result = str;
    result.replace(pos, from.length(), to);
    return result;
}

std::string StringUtil::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

bool StringUtil::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    if (start >= str.length()) return false;
    
    bool hasDot = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isAlpha(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isalpha);
}

bool StringUtil::isAlphaNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isalnum);
}

bool StringUtil::isEmpty(const std::string& str) {
    return str.empty();
}

bool StringUtil::isWhitespace(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isspace);
}

std::string StringUtil::format(const std::string& format, const std::vector<std::string>& args) {
    std::string result = format;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string placeholder = "{" + std::to_string(i) + "}";
        result = replaceAll(result, placeholder, args[i]);
    }
    
    return result;
}

std::string StringUtil::padLeft(const std::string& str, size_t width, char padChar) {
    if (str.length() >= width) return str;
    
    return std::string(width - str.length(), padChar) + str;
}

std::string StringUtil::padRight(const std::string& str, size_t width, char padChar) {
    if (str.length() >= width) return str;
    
    return str + std::string(width - str.length(), padChar);
}

std::string StringUtil::escapeHtml(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (char c : str) {
        result += charsToEscapeHtml(c);
    }
    
    return result;
}

std::string StringUtil::escapeJson(const std::string& str) {
    std::string result;
    result.reserve(str.length() + 10);
    
    for (char c : str) {
        result += charsToEscapeJson(c);
    }
    
    return result;
}

std::string StringUtil::escapeRegex(const std::string& str) {
    std::string specialChars = "\\^$.*+?()[]{}|";
    std::string result = str;
    
    for (char c : specialChars) {
        std::string search = std::string(1, c);
        std::string replace = "\\" + search;
        result = replaceAll(result, search, replace);
    }
    
    return result;
}

std::string StringUtil::unescapeHtml(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "&lt;", "<");
    result = replaceAll(result, "&gt;", ">");
    result = replaceAll(result, "&amp;", "&");
    result = replaceAll(result, "&quot;", "\"");
    result = replaceAll(result, "&#39;", "'");
    return result;
}

int StringUtil::parseInt(const std::string& str, int defaultValue) {
    try {
        return std::stoi(str);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

double StringUtil::parseDouble(const std::string& str, double defaultValue) {
    try {
        return std::stod(str);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

bool StringUtil::parseBool(const std::string& str, bool defaultValue) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes") return true;
    if (lower == "false" || lower == "0" || lower == "no") return false;
    return defaultValue;
}

std::string StringUtil::generateId(size_t length) {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, chars.length() - 1);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    
    return result;
}

std::string StringUtil::generateUuid() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    int i;
    
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    }
    
    return ss.str();
}

std::string StringUtil::repeat(const std::string& str, size_t count) {
    std::string result;
    result.reserve(str.length() * count);
    
    for (size_t i = 0; i < count; ++i) {
        result += str;
    }
    
    return result;
}

bool StringUtil::matches(const std::string& str, const std::string& pattern) {
    try {
        std::regex regexPattern(pattern);
        return std::regex_match(str, regexPattern);
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> StringUtil::extractMatches(const std::string& str, const std::string& pattern) {
    std::vector<std::string> matches;
    
    try {
        std::regex regexPattern(pattern);
        std::sregex_iterator iter(str.begin(), str.end(), regexPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            matches.push_back(iter->str());
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    
    return matches;
}

std::string StringUtil::normalizeLiteral(const std::string& str) {
    return trim(str);
}

std::string StringUtil::escapeLiteral(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "\\", "\\\\");
    result = replaceAll(result, "\"", "\\\"");
    result = replaceAll(result, "\n", "\\n");
    result = replaceAll(result, "\r", "\\r");
    result = replaceAll(result, "\t", "\\t");
    return result;
}

std::string StringUtil::unescapeLiteral(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "\\n", "\n");
    result = replaceAll(result, "\\r", "\r");
    result = replaceAll(result, "\\t", "\t");
    result = replaceAll(result, "\\\"", "\"");
    result = replaceAll(result, "\\\\", "\\");
    return result;
}

bool StringUtil::isUnquotedLiteral(const std::string& str) {
    return !str.empty() && str[0] != '"' && str[0] != '\'' && 
           !contains(str, " ") && !contains(str, "\t") && !contains(str, "\n");
}

std::string StringUtil::removeQuotes(const std::string& str) {
    if (str.length() >= 2 && 
        ((str[0] == '"' && str[str.length()-1] == '"') ||
         (str[0] == '\'' && str[str.length()-1] == '\''))) {
        return str.substr(1, str.length() - 2);
    }
    return str;
}

std::string StringUtil::extractCssValue(const std::string& str) {
    // Simple CSS value extraction
    std::string trimmed = trim(str);
    if (trimmed.empty()) return "";
    
    // Remove trailing semicolon if present
    if (trimmed.back() == ';') {
        trimmed.pop_back();
    }
    
    return trim(trimmed);
}

std::string StringUtil::extractJsExpression(const std::string& str) {
    return trim(str);
}

bool StringUtil::isValidCssProperty(const std::string& str) {
    // Basic CSS property validation
    if (str.empty()) return false;
    
    // Must start with letter or dash
    if (!std::isalpha(str[0]) && str[0] != '-') return false;
    
    // Rest must be alphanumeric or dash
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '-') return false;
    }
    
    return true;
}

bool StringUtil::isValidJsIdentifier(const std::string& str) {
    if (str.empty()) return false;
    
    // First character must be letter, underscore, or dollar sign
    if (!std::isalpha(str[0]) && str[0] != '_' && str[0] != '$') return false;
    
    // Rest must be alphanumeric, underscore, or dollar sign
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_' && str[i] != '$') return false;
    }
    
    return true;
}

std::string StringUtil::charsToEscapeHtml(const char c) {
    switch (c) {
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '&': return "&amp;";
        case '"': return "&quot;";
        case '\'': return "&#39;";
        default: return std::string(1, c);
    }
}

std::string StringUtil::charsToEscapeJson(const char c) {
    switch (c) {
        case '"': return "\\\"";
        case '\\': return "\\\\";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        default: return std::string(1, c);
    }
}

} // namespace CHTL::Util