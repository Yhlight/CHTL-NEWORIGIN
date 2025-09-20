#pragma once

#include "CHTLCommon.h"
#include "Token.h"
#include "ASTNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// Code fragment types
enum class FragmentType {
    CHTL,
    CHTLJS,
    CSS,
    JS,
    UNKNOWN
};

// Code fragment structure
struct CodeFragment {
    FragmentType type;
    String content;
    String originalContent;
    size_t startPosition;
    size_t endPosition;
    int line;
    int column;
    StringMap metadata;
    StringVector placeholders;
    
    CodeFragment(FragmentType t = FragmentType::UNKNOWN, const String& c = "")
        : type(t), content(c), originalContent(c), startPosition(0), endPosition(0), 
          line(0), column(0) {}
    
    bool isEmpty() const { return content.empty(); }
    bool hasPlaceholders() const { return !placeholders.empty(); }
    String toString() const;
};

// Placeholder management
class PlaceholderManager {
private:
    std::unordered_map<String, String> placeholderMap;
    std::unordered_map<String, String> reverseMap;
    int counter;
    
public:
    PlaceholderManager() : counter(0) {}
    
    String createPlaceholder(const String& type, const String& content = "");
    String getPlaceholder(const String& type, const String& content = "");
    String resolvePlaceholder(const String& placeholder) const;
    String resolveContent(const String& content) const;
    void clear();
    
    const std::unordered_map<String, String>& getPlaceholderMap() const { return placeholderMap; }
    const std::unordered_map<String, String>& getReverseMap() const { return reverseMap; }
};

// Unified Scanner - Core component for separating mixed code
class UnifiedScanner {
private:
    String source;
    String filename;
    std::vector<CodeFragment> fragments;
    PlaceholderManager placeholderManager;
    
    // Scanner state
    bool inGlobalStyle;
    bool inLocalScript;
    bool inLocalStyle;
    bool inTemplate;
    bool inCustom;
    bool inOrigin;
    bool inImport;
    bool inNamespace;
    bool inConfiguration;
    
    // Context tracking
    int braceDepth;
    int parenDepth;
    int bracketDepth;
    String currentContext;
    StringVector contextStack;
    
    // CHTL JS specific state
    bool inCHTLJSFunction;
    String currentFunction;
    StringVector functionStack;
    
public:
    UnifiedScanner(const String& src, const String& file = "");
    
    // Main scanning methods
    std::vector<CodeFragment> scan();
    std::vector<CodeFragment> scanFile(const String& filepath);
    
    // Fragment processing
    void processFragments();
    void mergeFragments();
    void validateFragments();
    
    // Context management
    void enterContext(const String& context);
    void exitContext();
    String getCurrentContext() const;
    bool isInContext(const String& context) const;
    
    // State management
    void reset();
    void setSource(const String& src, const String& file = "");
    
    // Getters
    const std::vector<CodeFragment>& getFragments() const { return fragments; }
    const PlaceholderManager& getPlaceholderManager() const { return placeholderManager; }
    
    // Error handling
    void reportError(const String& message, int line = -1, int col = -1);
    
private:
    // Core scanning algorithms
    void scanWide(); // Wide judgment for large CHTL blocks
    void scanStrict(); // Strict judgment for mixed JS/CHTLJS
    
    // Fragment type detection
    FragmentType detectFragmentType(const String& content, size_t position);
    bool isCHTLSyntax(const String& content, size_t position);
    bool isCHTLJSSyntax(const String& content, size_t position);
    bool isCSSSyntax(const String& content, size_t position);
    bool isJSSyntax(const String& content, size_t position);
    
    // Boundary detection
    size_t findBlockBoundary(const String& content, size_t start);
    size_t findStyleBlockBoundary(const String& content, size_t start);
    size_t findScriptBlockBoundary(const String& content, size_t start);
    size_t findTemplateBlockBoundary(const String& content, size_t start);
    size_t findCustomBlockBoundary(const String& content, size_t start);
    size_t findOriginBlockBoundary(const String& content, size_t start);
    size_t findImportBlockBoundary(const String& content, size_t start);
    size_t findNamespaceBlockBoundary(const String& content, size_t start);
    size_t findConfigurationBlockBoundary(const String& content, size_t start);
    
    // CHTL JS specific scanning
    void scanCHTLJSFunction(const String& content, size_t& position);
    void scanCHTLJSExpression(const String& content, size_t& position);
    void scanCHTLJSSyntax(const String& content, size_t& position);
    
    // Placeholder mechanism
    String replaceWithPlaceholders(const String& content, FragmentType targetType);
    String restoreFromPlaceholders(const String& content);
    
    // Syntax boundary recognition
    bool isSyntaxBoundary(const String& content, size_t position);
    bool isCHTLBoundary(const String& content, size_t position);
    bool isCHTLJSBoundary(const String& content, size_t position);
    bool isJSBoundary(const String& content, size_t position);
    bool isCSSBoundary(const String& content, size_t position);
    
    // Helper methods
    void updateScannerState(const String& content, size_t position);
    bool isInString(const String& content, size_t position);
    bool isInComment(const String& content, size_t position);
    bool isInTemplate(const String& content, size_t position);
    char getCharAt(const String& content, size_t position);
    char peekChar(const String& content, size_t position, int offset = 1);
    
    // Fragment end detection
    size_t findFragmentEnd(const String& content, size_t start, FragmentType type);
    size_t findCHTLFragmentEnd(const String& content, size_t start);
    size_t findCHTLJSFragmentEnd(const String& content, size_t start);
    size_t findCSSFragmentEnd(const String& content, size_t start);
    size_t findJSFragmentEnd(const String& content, size_t start);
    
    // Dynamic slice expansion and smart backtracking
    size_t expandSlice(const String& content, size_t start, size_t end, FragmentType type);
    size_t backtrackSlice(const String& content, size_t start, size_t end, FragmentType type);
    
    // Variable length slicing
    std::pair<size_t, size_t> getVariableLengthSlice(const String& content, size_t start, FragmentType type);
};

// Fragment processor for specific types
class FragmentProcessor {
public:
    static CodeFragment processCHTLFragment(const CodeFragment& fragment);
    static CodeFragment processCHTLJSFragment(const CodeFragment& fragment);
    static CodeFragment processCSSFragment(const CodeFragment& fragment);
    static CodeFragment processJSFragment(const CodeFragment& fragment);
    
    static String extractCHTLSyntax(const String& content);
    static String extractCHTLJSSyntax(const String& content);
    static String extractCSSSyntax(const String& content);
    static String extractJSSyntax(const String& content);
};

} // namespace CHTL