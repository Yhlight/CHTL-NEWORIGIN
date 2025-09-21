#include "../../include/UnifiedScanner.h"
#include <algorithm>
#include <regex>
#include <stack>

namespace CHTL {

UnifiedScanner::UnifiedScanner(const String& src, const String& file)
    : source(src), filename(file), inGlobalStyle(false), inLocalScript(false),
      inLocalStyle(false), inTemplate(false), inCustom(false), inOrigin(false),
      inImport(false), inNamespace(false), inConfiguration(false),
      braceDepth(0), parenDepth(0), bracketDepth(0), inCHTLJSFunction(false) {
}

std::vector<CodeFragment> UnifiedScanner::scan() {
    fragments.clear();
    placeholderManager.clear();
    reset();
    
    size_t position = 0;
    size_t contentLength = source.length();
    
    while (position < contentLength) {
        // Update scanner state based on current position
        updateScannerState(source, position);
        
        // Determine fragment type
        FragmentType type = detectFragmentType(source, position);
        
        // Find the end of this fragment
        size_t endPosition = findFragmentEnd(source, position, type);
        
        // Create fragment
        String content = source.substr(position, endPosition - position);
        CodeFragment fragment(type, content);
        fragment.startPosition = position;
        fragment.endPosition = endPosition;
        
        // Extract line and column information
        size_t lineStart = source.rfind('\n', position);
        fragment.line = std::count(source.begin(), source.begin() + position, '\n') + 1;
        fragment.column = position - lineStart;
        
        fragments.push_back(fragment);
        position = endPosition;
    }
    
    // Process fragments to apply placeholder mechanism
    processFragments();
    
    return fragments;
}

std::vector<CodeFragment> UnifiedScanner::scanFile(const String& filepath) {
    String content = FileSystem::readFile(filepath);
    setSource(content, filepath);
    return scan();
}

void UnifiedScanner::processFragments() {
    for (auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL:
                fragment = FragmentProcessor::processCHTLFragment(fragment);
                break;
            case FragmentType::CHTLJS:
                fragment = FragmentProcessor::processCHTLJSFragment(fragment);
                break;
            case FragmentType::CSS:
                fragment = FragmentProcessor::processCSSFragment(fragment);
                break;
            case FragmentType::JS:
                fragment = FragmentProcessor::processJSFragment(fragment);
                break;
            default:
                break;
        }
    }
}

void UnifiedScanner::mergeFragments() {
    // Merge adjacent fragments of the same type
    std::vector<CodeFragment> merged;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        if (merged.empty() || merged.back().type != fragments[i].type) {
            merged.push_back(fragments[i]);
        } else {
            // Merge with previous fragment
            merged.back().content += fragments[i].content;
            merged.back().endPosition = fragments[i].endPosition;
            merged.back().metadata.insert(fragments[i].metadata.begin(), fragments[i].metadata.end());
        }
    }
    
    fragments = merged;
}

void UnifiedScanner::validateFragments() {
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::UNKNOWN) {
            reportError("Unknown fragment type detected", fragment.line, fragment.column);
        }
    }
}

void UnifiedScanner::enterContext(const String& context) {
    contextStack.push_back(currentContext);
    currentContext = context;
}

void UnifiedScanner::exitContext() {
    if (!contextStack.empty()) {
        currentContext = contextStack.back();
        contextStack.pop_back();
    } else {
        currentContext = "";
    }
}

String UnifiedScanner::getCurrentContext() const {
    return currentContext;
}

bool UnifiedScanner::isInContext(const String& context) const {
    return currentContext == context;
}

void UnifiedScanner::reset() {
    inGlobalStyle = false;
    inLocalScript = false;
    inLocalStyle = false;
    inTemplate = false;
    inCustom = false;
    inOrigin = false;
    inImport = false;
    inNamespace = false;
    inConfiguration = false;
    
    braceDepth = 0;
    parenDepth = 0;
    bracketDepth = 0;
    
    inCHTLJSFunction = false;
    currentFunction = "";
    functionStack.clear();
    
    currentContext = "";
    contextStack.clear();
}

void UnifiedScanner::setSource(const String& src, const String& file) {
    source = src;
    filename = file;
    reset();
}

void UnifiedScanner::reportError(const String& message, int line, int col) {
    String error = "UnifiedScanner Error";
    if (!filename.empty()) {
        error += " in " + filename;
    }
    if (line > 0) {
        error += " at line " + std::to_string(line);
        if (col > 0) {
            error += ", column " + std::to_string(col);
        }
    }
    error += ": " + message;
    
    throw CHTLException(error, line, col);
}

FragmentType UnifiedScanner::detectFragmentType(const String& content, size_t position) {
    // Check for CHTL syntax boundaries
    if (isCHTLSyntax(content, position)) {
        return FragmentType::CHTL;
    }
    
    // Check for CHTL JS syntax
    if (isCHTLJSSyntax(content, position)) {
        return FragmentType::CHTLJS;
    }
    
    // Check for CSS syntax
    if (isCSSSyntax(content, position)) {
        return FragmentType::CSS;
    }
    
    // Check for JS syntax
    if (isJSSyntax(content, position)) {
        return FragmentType::JS;
    }
    
    return FragmentType::UNKNOWN;
}

bool UnifiedScanner::isCHTLSyntax(const String& content, size_t position) {
    // Check for CHTL keywords and syntax patterns
    static const std::vector<String> chtlKeywords = {
        "text", "style", "script", "template", "custom", "origin",
        "import", "namespace", "configuration", "inherit", "delete",
        "insert", "from", "as", "except", "use", "html5"
    };
    
    // Check for CHTL block patterns
    static const std::regex chtlBlockPattern(R"(\b(text|style|script|template|custom|origin|import|namespace|configuration)\s*\{)");
    static const std::regex chtlSpecialPattern(R"(\[(\w+)\])");
    
    std::smatch match;
    String substr = content.substr(position, std::min(size_t(100), content.length() - position));
    
    // Check for CHTL block patterns
    if (std::regex_search(substr, match, chtlBlockPattern)) {
        return true;
    }
    
    // Check for CHTL special syntax patterns
    if (std::regex_search(substr, match, chtlSpecialPattern)) {
        return true;
    }
    
    // Check for CHTL operators and symbols
    if (position < content.length()) {
        char c = content[position];
        if (c == '@' || c == '#' || c == '$' || c == '&') {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSSyntax(const String& content, size_t position) {
    // Check for CHTL JS specific syntax patterns
    static const std::vector<String> chtljsKeywords = {
        "Listen", "Animate", "Router", "Delegate", "Vir", "ScriptLoader"
    };
    
    static const std::regex chtljsPattern(R"(\b(Listen|Animate|Router|Delegate|Vir|ScriptLoader)\s*\{)");
    static const std::regex selectorPattern(R"(\{\{([^}]+)\}\})");
    static const std::regex arrowPattern(R"(\-\>)");
    
    String substr = content.substr(position, std::min(size_t(100), content.length() - position));
    std::smatch match;
    
    // Check for CHTL JS function patterns
    if (std::regex_search(substr, match, chtljsPattern)) {
        return true;
    }
    
    // Check for selector patterns
    if (std::regex_search(substr, match, selectorPattern)) {
        return true;
    }
    
    // Check for arrow operator
    if (std::regex_search(substr, match, arrowPattern)) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCSSSyntax(const String& content, size_t position) {
    // Check for CSS patterns
    static const std::regex cssSelectorPattern(R"([.#]?\w+\s*\{)");
    static const std::regex cssPropertyPattern(R"(\w+\s*:\s*[^;]+;)");
    static const std::regex cssAtRulePattern(R"(@\w+)");
    
    String substr = content.substr(position, std::min(size_t(100), content.length() - position));
    std::smatch match;
    
    // Check for CSS selector patterns
    if (std::regex_search(substr, match, cssSelectorPattern)) {
        return true;
    }
    
    // Check for CSS property patterns
    if (std::regex_search(substr, match, cssPropertyPattern)) {
        return true;
    }
    
    // Check for CSS at-rules
    if (std::regex_search(substr, match, cssAtRulePattern)) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isJSSyntax(const String& content, size_t position) {
    // Check for JavaScript patterns
    static const std::regex jsFunctionPattern(R"(\bfunction\s+\w+\s*\()");
    static const std::regex jsVarPattern(R"(\b(var|let|const)\s+\w+)");
    static const std::regex jsArrowPattern(R"(=>\s*\{?)");
    static const std::regex jsClassPattern(R"(\bclass\s+\w+)");
    
    String substr = content.substr(position, std::min(size_t(100), content.length() - position));
    std::smatch match;
    
    // Check for JavaScript function patterns
    if (std::regex_search(substr, match, jsFunctionPattern)) {
        return true;
    }
    
    // Check for variable declarations
    if (std::regex_search(substr, match, jsVarPattern)) {
        return true;
    }
    
    // Check for arrow functions
    if (std::regex_search(substr, match, jsArrowPattern)) {
        return true;
    }
    
    // Check for class declarations
    if (std::regex_search(substr, match, jsClassPattern)) {
        return true;
    }
    
    return false;
}

size_t UnifiedScanner::findFragmentEnd(const String& content, size_t start, FragmentType type) {
    switch (type) {
        case FragmentType::CHTL:
            return findCHTLFragmentEnd(content, start);
        case FragmentType::CHTLJS:
            return findCHTLJSFragmentEnd(content, start);
        case FragmentType::CSS:
            return findCSSFragmentEnd(content, start);
        case FragmentType::JS:
            return findJSFragmentEnd(content, start);
        default:
            return start + 1; // Single character for unknown
    }
}

size_t UnifiedScanner::findCHTLFragmentEnd(const String& content, size_t start) {
    // Find the end of CHTL block or expression
    size_t position = start;
    int braceCount = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (position < content.length()) {
        char c = content[position];
        
        if (!inString) {
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount == 0) {
                    return position + 1;
                }
            } else if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            }
        } else {
            if (c == stringDelimiter && content[position - 1] != '\\') {
                inString = false;
            }
        }
        
        position++;
    }
    
    return content.length();
}

size_t UnifiedScanner::findCHTLJSFragmentEnd(const String& content, size_t start) {
    // Similar to CHTL but with different boundary detection
    return findCHTLFragmentEnd(content, start); // Simplified for now
}

size_t UnifiedScanner::findCSSFragmentEnd(const String& content, size_t start) {
    // Find end of CSS block
    return findCHTLFragmentEnd(content, start); // Simplified for now
}

size_t UnifiedScanner::findJSFragmentEnd(const String& content, size_t start) {
    // Find end of JavaScript statement or block
    return findCHTLFragmentEnd(content, start); // Simplified for now
}

void UnifiedScanner::updateScannerState(const String& content, size_t position) {
    // Update scanner state based on current position
    // This is a simplified implementation
    
    if (position >= content.length()) return;
    
    char c = content[position];
    
    // Update brace depth
    if (c == '{') braceDepth++;
    else if (c == '}') braceDepth--;
    
    // Update parenthesis depth
    if (c == '(') parenDepth++;
    else if (c == ')') parenDepth--;
    
    // Update bracket depth
    if (c == '[') bracketDepth++;
    else if (c == ']') bracketDepth--;
    
    // Update context based on keywords
    String substr = content.substr(position, 20);
    if (substr.find("style") == 0) {
        inLocalStyle = true;
    } else if (substr.find("script") == 0) {
        inLocalScript = true;
    }
}

// PlaceholderManager implementation
String PlaceholderManager::createPlaceholder(const String& type, const String& content) {
    String placeholder = "_" + type + "_PLACEHOLDER_" + std::to_string(++counter) + "_";
    placeholderMap[placeholder] = content;
    reverseMap[content] = placeholder;
    return placeholder;
}

String PlaceholderManager::getPlaceholder(const String& type, const String& content) {
    auto it = reverseMap.find(content);
    if (it != reverseMap.end()) {
        return it->second;
    }
    return createPlaceholder(type, content);
}

String PlaceholderManager::resolvePlaceholder(const String& placeholder) const {
    auto it = placeholderMap.find(placeholder);
    return (it != placeholderMap.end()) ? it->second : placeholder;
}

String PlaceholderManager::resolveContent(const String& content) const {
    String result = content;
    for (const auto& pair : placeholderMap) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != String::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    return result;
}

void PlaceholderManager::clear() {
    placeholderMap.clear();
    reverseMap.clear();
    counter = 0;
}

// FragmentProcessor implementation
CodeFragment FragmentProcessor::processCHTLFragment(const CodeFragment& fragment) {
    CodeFragment processed = fragment;
    processed.content = extractCHTLSyntax(fragment.content);
    return processed;
}

CodeFragment FragmentProcessor::processCHTLJSFragment(const CodeFragment& fragment) {
    CodeFragment processed = fragment;
    processed.content = extractCHTLJSSyntax(fragment.content);
    return processed;
}

CodeFragment FragmentProcessor::processCSSFragment(const CodeFragment& fragment) {
    CodeFragment processed = fragment;
    processed.content = extractCSSSyntax(fragment.content);
    return processed;
}

CodeFragment FragmentProcessor::processJSFragment(const CodeFragment& fragment) {
    CodeFragment processed = fragment;
    processed.content = extractJSSyntax(fragment.content);
    return processed;
}

String FragmentProcessor::extractCHTLSyntax(const String& content) {
    // Extract and preserve CHTL-specific syntax
    return content; // Simplified for now
}

String FragmentProcessor::extractCHTLJSSyntax(const String& content) {
    // Extract and preserve CHTL JS-specific syntax
    return content; // Simplified for now
}

String FragmentProcessor::extractCSSSyntax(const String& content) {
    // Extract and preserve CSS syntax
    return content; // Simplified for now
}

String FragmentProcessor::extractJSSyntax(const String& content) {
    // Extract and preserve JavaScript syntax
    return content; // Simplified for now
}

} // namespace CHTL