#include "GlobalMap.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// GlobalMap implementation
GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}

GlobalMap::GlobalMap() {
    initializeDefaults();
}

void GlobalMap::registerKeyword(const std::string& name, TokenType type) {
    keywordMap_[name] = type;
}

void GlobalMap::registerKeywordGroup(const std::string& groupName, const std::vector<std::string>& keywords) {
    keywordGroups_[groupName] = keywords;
    
    // Register each keyword in the group
    for (const auto& keyword : keywords) {
        // Assign appropriate token type based on group
        TokenType type = TokenType::IDENTIFIER; // Default
        if (groupName == "CUSTOM_STYLE") type = TokenType::CHTL_CUSTOM_CALL;
        else if (groupName == "CUSTOM_ELEMENT") type = TokenType::CHTL_CUSTOM_CALL;
        else if (groupName == "CUSTOM_VAR") type = TokenType::CHTL_CUSTOM_CALL;
        else if (groupName == "TEMPLATE_STYLE") type = TokenType::CHTL_TEMPLATE_CALL;
        else if (groupName == "TEMPLATE_ELEMENT") type = TokenType::CHTL_TEMPLATE_CALL;
        else if (groupName == "TEMPLATE_VAR") type = TokenType::CHTL_TEMPLATE_CALL;
        else if (groupName == "ORIGIN_HTML") type = TokenType::CHTL_ORIGIN_CALL;
        else if (groupName == "ORIGIN_STYLE") type = TokenType::CHTL_ORIGIN_CALL;
        else if (groupName == "ORIGIN_JAVASCRIPT") type = TokenType::CHTL_ORIGIN_CALL;
        else if (groupName == "IMPORT_HTML") type = TokenType::CHTL_IMPORT_CALL;
        else if (groupName == "IMPORT_STYLE") type = TokenType::CHTL_IMPORT_CALL;
        else if (groupName == "IMPORT_JAVASCRIPT") type = TokenType::CHTL_IMPORT_CALL;
        else if (groupName == "IMPORT_CHTL") type = TokenType::CHTL_IMPORT_CALL;
        else if (groupName == "IMPORT_CRMOD") type = TokenType::CHTL_IMPORT_CALL;
        
        registerKeyword(keyword, type);
    }
}

void GlobalMap::unregisterKeyword(const std::string& name) {
    keywordMap_.erase(name);
}

void GlobalMap::registerSymbol(const std::string& symbol, TokenType type) {
    symbolMap_[symbol] = type;
}

void GlobalMap::unregisterSymbol(const std::string& symbol) {
    symbolMap_.erase(symbol);
}

TokenType GlobalMap::getTokenType(const std::string& identifier) const {
    // Check keywords first
    auto keywordIt = keywordMap_.find(identifier);
    if (keywordIt != keywordMap_.end()) {
        return keywordIt->second;
    }
    
    // Check symbols
    auto symbolIt = symbolMap_.find(identifier);
    if (symbolIt != symbolMap_.end()) {
        return symbolIt->second;
    }
    
    // Default to identifier
    return TokenType::IDENTIFIER;
}

bool GlobalMap::isKeyword(const std::string& identifier) const {
    return keywordMap_.find(identifier) != keywordMap_.end();
}

bool GlobalMap::isSymbol(const std::string& identifier) const {
    return symbolMap_.find(identifier) != symbolMap_.end();
}

bool GlobalMap::isOperator(const std::string& identifier) const {
    TokenType type = getTokenType(identifier);
    return type >= TokenType::OPERATOR_COLON && type <= TokenType::OPERATOR_QUESTION;
}

bool GlobalMap::isDelimiter(const std::string& identifier) const {
    TokenType type = getTokenType(identifier);
    return type >= TokenType::BRACE_OPEN && type <= TokenType::ANGLE_CLOSE;
}

void GlobalMap::loadConfiguration(const std::map<std::string, std::vector<std::string>>& config) {
    // Clear existing configuration
    resetToDefaults();
    
    // Load new configuration
    for (const auto& [groupName, keywords] : config) {
        registerKeywordGroup(groupName, keywords);
    }
}

void GlobalMap::resetToDefaults() {
    keywordMap_.clear();
    keywordGroups_.clear();
    symbolMap_.clear();
    initializeDefaults();
}

void GlobalMap::initializeCHTLKeywords() {
    // Core CHTL keywords
    registerKeyword("use", TokenType::KEYWORD_USE);
    registerKeyword("html5", TokenType::KEYWORD_HTML5);
    registerKeyword("text", TokenType::KEYWORD_TEXT);
    registerKeyword("style", TokenType::KEYWORD_STYLE);
    registerKeyword("script", TokenType::KEYWORD_SCRIPT);
    
    // Template keywords
    registerKeyword("Custom", TokenType::KEYWORD_CUSTOM);
    registerKeyword("Template", TokenType::KEYWORD_TEMPLATE);
    registerKeyword("Origin", TokenType::KEYWORD_ORIGIN);
    registerKeyword("Import", TokenType::KEYWORD_IMPORT);
    registerKeyword("Namespace", TokenType::KEYWORD_NAMESPACE);
    
    // Control keywords
    registerKeyword("inherit", TokenType::KEYWORD_INHERIT);
    registerKeyword("delete", TokenType::KEYWORD_DELETE);
    registerKeyword("insert", TokenType::KEYWORD_INSERT);
    registerKeyword("after", TokenType::KEYWORD_AFTER);
    registerKeyword("before", TokenType::KEYWORD_BEFORE);
    registerKeyword("replace", TokenType::KEYWORD_REPLACE);
    registerKeyword("at", TokenType::KEYWORD_ATTOP); // Simplified
    registerKeyword("from", TokenType::KEYWORD_FROM);
    registerKeyword("as", TokenType::KEYWORD_AS);
    registerKeyword("except", TokenType::KEYWORD_EXCEPT);
    
    // Conditional keywords
    registerKeyword("if", TokenType::KEYWORD_IF);
    registerKeyword("else", TokenType::KEYWORD_ELSE);
    registerKeyword("elseif", TokenType::KEYWORD_ELSEIF);
    
    // Template calls
    registerKeyword("@Style", TokenType::CHTL_TEMPLATE_CALL);
    registerKeyword("@Element", TokenType::CHTL_TEMPLATE_CALL);
    registerKeyword("@Var", TokenType::CHTL_TEMPLATE_CALL);
    registerKeyword("@Html", TokenType::CHTL_ORIGIN_CALL);
    registerKeyword("@JavaScript", TokenType::CHTL_ORIGIN_CALL);
    registerKeyword("@Chtl", TokenType::CHTL_IMPORT_CALL);
    registerKeyword("@CJmod", TokenType::CHTL_IMPORT_CALL);
    registerKeyword("@Config", TokenType::CHTL_CONFIG_CALL);
}

void GlobalMap::initializeCHTLJSSymbols() {
    // CHTL JS specific symbols
    registerSymbol("{{", TokenType::CJMOD_DOUBLE_BRACE_OPEN);
    registerSymbol("}}", TokenType::CJMOD_DOUBLE_BRACE_CLOSE);
    registerSymbol("->", TokenType::OPERATOR_ARROW);
    registerSymbol("&->", TokenType::CJMOD_DELEGATE);
    
    // CHTL JS keywords
    registerKeyword("Vir", TokenType::CJMOD_VIRTUAL_OBJECT);
    registerKeyword("Listen", TokenType::CJMOD_LISTEN);
    registerKeyword("Animate", TokenType::CJMOD_ANIMATE);
    registerKeyword("Router", TokenType::CJMOD_ROUTER);
    registerKeyword("Delegate", TokenType::CJMOD_DELEGATE);
    registerKeyword("ScriptLoader", TokenType::CJMOD_FUNCTION_CALL);
    registerKeyword("printMylove", TokenType::CJMOD_FUNCTION_CALL);
    registerKeyword("iNeverAway", TokenType::CJMOD_FUNCTION_CALL);
}

void GlobalMap::initializeDefaults() {
    initializeOperators();
    initializeDelimiters();
    initializeSpecialSymbols();
    initializeCHTLKeywords();
    initializeCHTLJSSymbols();
}

void GlobalMap::initializeOperators() {
    registerSymbol(":", TokenType::OPERATOR_COLON);
    registerSymbol("=", TokenType::OPERATOR_EQUALS);
    registerSymbol("+", TokenType::OPERATOR_PLUS);
    registerSymbol("-", TokenType::OPERATOR_MINUS);
    registerSymbol("*", TokenType::OPERATOR_MULTIPLY);
    registerSymbol("/", TokenType::OPERATOR_DIVIDE);
    registerSymbol("%", TokenType::OPERATOR_MODULO);
    registerSymbol("**", TokenType::OPERATOR_POWER);
    registerSymbol("&&", TokenType::OPERATOR_AND);
    registerSymbol("||", TokenType::OPERATOR_OR);
    registerSymbol("!", TokenType::OPERATOR_NOT);
    registerSymbol("<", TokenType::OPERATOR_LESS);
    registerSymbol(">", TokenType::OPERATOR_GREATER);
    registerSymbol("<=", TokenType::OPERATOR_LESS_EQUAL);
    registerSymbol(">=", TokenType::OPERATOR_GREATER_EQUAL);
    registerSymbol("==", TokenType::OPERATOR_EQUAL);
    registerSymbol("!=", TokenType::OPERATOR_NOT_EQUAL);
    registerSymbol(".", TokenType::OPERATOR_DOT);
    registerSymbol(",", TokenType::OPERATOR_COMMA);
    registerSymbol(";", TokenType::OPERATOR_SEMICOLON);
    registerSymbol("?", TokenType::OPERATOR_QUESTION);
}

void GlobalMap::initializeDelimiters() {
    registerSymbol("{", TokenType::BRACE_OPEN);
    registerSymbol("}", TokenType::BRACE_CLOSE);
    registerSymbol("(", TokenType::PAREN_OPEN);
    registerSymbol(")", TokenType::PAREN_CLOSE);
    registerSymbol("[", TokenType::BRACKET_OPEN);
    registerSymbol("]", TokenType::BRACKET_CLOSE);
    registerSymbol("<", TokenType::ANGLE_OPEN);
    registerSymbol(">", TokenType::ANGLE_CLOSE);
}

void GlobalMap::initializeSpecialSymbols() {
    registerSymbol("@", TokenType::AT_SYMBOL);
    registerSymbol("#", TokenType::HASH_SYMBOL);
    registerSymbol("$", TokenType::DOLLAR_SYMBOL);
    registerSymbol("&", TokenType::AMPERSAND_SYMBOL);
    registerSymbol("%", TokenType::PERCENT_SYMBOL);
}

std::vector<std::string> GlobalMap::getKeywords() const {
    std::vector<std::string> keywords;
    for (const auto& [name, type] : keywordMap_) {
        keywords.push_back(name);
    }
    return keywords;
}

std::vector<std::string> GlobalMap::getSymbols() const {
    std::vector<std::string> symbols;
    for (const auto& [symbol, type] : symbolMap_) {
        symbols.push_back(symbol);
    }
    return symbols;
}

std::vector<std::string> GlobalMap::getOperators() const {
    std::vector<std::string> operators;
    for (const auto& [symbol, type] : symbolMap_) {
        if (type >= TokenType::OPERATOR_COLON && type <= TokenType::OPERATOR_QUESTION) {
            operators.push_back(symbol);
        }
    }
    return operators;
}

std::vector<std::string> GlobalMap::getDelimiters() const {
    std::vector<std::string> delimiters;
    for (const auto& [symbol, type] : symbolMap_) {
        if (type >= TokenType::BRACE_OPEN && type <= TokenType::ANGLE_CLOSE) {
            delimiters.push_back(symbol);
        }
    }
    return delimiters;
}

bool GlobalMap::isValidIdentifier(const std::string& identifier) const {
    if (identifier.empty()) return false;
    
    // Check if it's a reserved keyword
    if (isReservedKeyword(identifier)) return false;
    
    // Check basic identifier rules
    if (!Token::isIdentifierStart(identifier[0])) return false;
    
    for (size_t i = 1; i < identifier.length(); ++i) {
        if (!Token::isIdentifierPart(identifier[i])) return false;
    }
    
    return true;
}

bool GlobalMap::isReservedKeyword(const std::string& identifier) const {
    // Core reserved keywords that cannot be overridden
    static const std::vector<std::string> reserved = {
        "use", "html5", "text", "style", "script", "if", "else", "elseif",
        "inherit", "delete", "insert", "after", "before", "replace", "at",
        "from", "as", "except"
    };
    
    return std::find(reserved.begin(), reserved.end(), identifier) != reserved.end();
}

void GlobalMap::printKeywords() const {
    std::cout << "Registered Keywords:\n";
    for (const auto& [name, type] : keywordMap_) {
        std::cout << "  " << name << " -> " << TokenUtils::tokenTypeToString(type) << "\n";
    }
}

void GlobalMap::printSymbols() const {
    std::cout << "Registered Symbols:\n";
    for (const auto& [symbol, type] : symbolMap_) {
        std::cout << "  " << symbol << " -> " << TokenUtils::tokenTypeToString(type) << "\n";
    }
}

std::string GlobalMap::getDebugInfo() const {
    std::ostringstream oss;
    oss << "GlobalMap Debug Info:\n";
    oss << "Keywords: " << keywordMap_.size() << "\n";
    oss << "Symbols: " << symbolMap_.size() << "\n";
    oss << "Keyword Groups: " << keywordGroups_.size() << "\n";
    return oss.str();
}

// NameConfigurationLoader implementation
std::map<std::string, std::vector<std::string>> NameConfigurationLoader::parseNameConfiguration(const std::string& configContent) {
    std::map<std::string, std::vector<std::string>> config;
    
    // Simple parsing - in real implementation, this would use proper CHTL parsing
    std::istringstream iss(configContent);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = Util::StringUtil::trim(line);
        if (line.empty() || line[0] == '/' || line[0] == '#') continue;
        
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) continue;
        
        std::string key = Util::StringUtil::trim(line.substr(0, equalPos));
        std::string value = Util::StringUtil::trim(line.substr(equalPos + 1));
        
        if (key.empty() || value.empty()) continue;
        
        config[key] = parseGroupValue(value);
    }
    
    return config;
}

void NameConfigurationLoader::applyConfiguration(GlobalMap& globalMap, const std::map<std::string, std::vector<std::string>>& config) {
    globalMap.loadConfiguration(config);
}

std::vector<std::string> NameConfigurationLoader::parseGroupValue(const std::string& value) {
    std::vector<std::string> result;
    
    // Handle array syntax [item1, item2, item3]
    if (value.front() == '[' && value.back() == ']') {
        std::string content = value.substr(1, value.length() - 2);
        std::istringstream iss(content);
        std::string item;
        
        while (std::getline(iss, item, ',')) {
            item = trimValue(item);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
    } else {
        // Single value
        result.push_back(trimValue(value));
    }
    
    return result;
}

std::string NameConfigurationLoader::trimValue(const std::string& value) {
    std::string trimmed = Util::StringUtil::trim(value);
    
    // Remove quotes if present
    if (trimmed.length() >= 2 && 
        ((trimmed.front() == '"' && trimmed.back() == '"') ||
         (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }
    
    return trimmed;
}

// KeywordValidator implementation
bool KeywordValidator::isValidKeywordName(const std::string& name) {
    if (name.empty()) return false;
    
    // Must start with letter or underscore
    if (!std::isalpha(name[0]) && name[0] != '_') return false;
    
    // Rest must be alphanumeric, underscore, or dash
    for (size_t i = 1; i < name.length(); ++i) {
        if (!std::isalnum(name[i]) && name[i] != '_' && name[i] != '-') {
            return false;
        }
    }
    
    return true;
}

bool KeywordValidator::isValidKeywordGroup(const std::vector<std::string>& keywords) {
    if (keywords.empty()) return false;
    
    for (const auto& keyword : keywords) {
        if (!isValidKeywordName(keyword)) {
            return false;
        }
    }
    
    return true;
}

bool KeywordValidator::hasConflict(const std::string& keyword, const std::vector<std::string>& existing) {
    return std::find(existing.begin(), existing.end(), keyword) != existing.end();
}

bool KeywordValidator::isValidCHTLKeyword(const std::string& keyword) {
    return isValidKeywordName(keyword) && !Util::StringUtil::startsWith(keyword, "@");
}

bool KeywordValidator::isValidCHTLJSKeyword(const std::string& keyword) {
    return isValidKeywordName(keyword) && !Util::StringUtil::startsWith(keyword, "{{");
}

bool KeywordValidator::isValidTemplateKeyword(const std::string& keyword) {
    return Util::StringUtil::startsWith(keyword, "@") && 
           keyword.length() > 1 &&
           isValidKeywordName(keyword.substr(1));
}

bool KeywordValidator::isValidCustomKeyword(const std::string& keyword) {
    return Util::StringUtil::startsWith(keyword, "@") && 
           keyword.length() > 1 &&
           isValidKeywordName(keyword.substr(1));
}

bool KeywordValidator::isValidOriginKeyword(const std::string& keyword) {
    return Util::StringUtil::startsWith(keyword, "@") && 
           keyword.length() > 1 &&
           isValidKeywordName(keyword.substr(1));
}

// SymbolResolver implementation
SymbolResolver::SymbolResolver(GlobalMap& globalMap) : globalMap_(globalMap) {
}

TokenType SymbolResolver::resolveIdentifier(const std::string& identifier) const {
    return globalMap_.getTokenType(identifier);
}

TokenType SymbolResolver::resolveOperator(const std::string& operatorStr) const {
    return globalMap_.getTokenType(operatorStr);
}

TokenType SymbolResolver::resolveDelimiter(const std::string& delimiter) const {
    return globalMap_.getTokenType(delimiter);
}

TokenType SymbolResolver::resolveSpecialSymbol(const std::string& symbol) const {
    return globalMap_.getTokenType(symbol);
}

TokenType SymbolResolver::resolveInContext(const std::string& identifier, const std::vector<Token>& context) const {
    if (isInTemplateContext(context)) {
        return resolveInTemplateContext(identifier);
    } else if (isInCustomContext(context)) {
        return resolveInCustomContext(identifier);
    } else if (isInOriginContext(context)) {
        return resolveInOriginContext(identifier);
    } else if (isInScriptContext(context)) {
        return resolveCHTLJSIdentifier(identifier);
    }
    
    return resolveIdentifier(identifier);
}

TokenType SymbolResolver::resolveInTemplateContext(const std::string& identifier) const {
    if (Util::StringUtil::startsWith(identifier, "@")) {
        return TokenType::CHTL_TEMPLATE_CALL;
    }
    return TokenType::IDENTIFIER;
}

TokenType SymbolResolver::resolveInCustomContext(const std::string& identifier) const {
    if (Util::StringUtil::startsWith(identifier, "@")) {
        return TokenType::CHTL_CUSTOM_CALL;
    }
    return TokenType::IDENTIFIER;
}

TokenType SymbolResolver::resolveInOriginContext(const std::string& identifier) const {
    if (Util::StringUtil::startsWith(identifier, "@")) {
        return TokenType::CHTL_ORIGIN_CALL;
    }
    return TokenType::IDENTIFIER;
}

TokenType SymbolResolver::resolveCHTLJSIdentifier(const std::string& identifier) const {
    TokenType type = globalMap_.getTokenType(identifier);
    
    // Check for CHTL JS specific keywords
    if (identifier == "Vir") return TokenType::CJMOD_VIRTUAL_OBJECT;
    if (identifier == "Listen") return TokenType::CJMOD_LISTEN;
    if (identifier == "Animate") return TokenType::CJMOD_ANIMATE;
    if (identifier == "Router") return TokenType::CJMOD_ROUTER;
    if (identifier == "Delegate") return TokenType::CJMOD_DELEGATE;
    
    return type;
}

TokenType SymbolResolver::resolveVirtualObject(const std::string& identifier) const {
    return TokenType::CJMOD_VIRTUAL_OBJECT;
}

TokenType SymbolResolver::resolveFunctionCall(const std::string& identifier) const {
    return TokenType::CJMOD_FUNCTION_CALL;
}

bool SymbolResolver::isInTemplateContext(const std::vector<Token>& context) const {
    for (const auto& token : context) {
        if (token.getType() == TokenType::KEYWORD_TEMPLATE) {
            return true;
        }
    }
    return false;
}

bool SymbolResolver::isInCustomContext(const std::vector<Token>& context) const {
    for (const auto& token : context) {
        if (token.getType() == TokenType::KEYWORD_CUSTOM) {
            return true;
        }
    }
    return false;
}

bool SymbolResolver::isInOriginContext(const std::vector<Token>& context) const {
    for (const auto& token : context) {
        if (token.getType() == TokenType::KEYWORD_ORIGIN) {
            return true;
        }
    }
    return false;
}

bool SymbolResolver::isInScriptContext(const std::vector<Token>& context) const {
    for (const auto& token : context) {
        if (token.getType() == TokenType::KEYWORD_SCRIPT) {
            return true;
        }
    }
    return false;
}

} // namespace CHTL