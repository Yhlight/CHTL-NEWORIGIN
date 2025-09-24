#pragma once

#include "ParserState.h"
#include "ParserStrategy.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLLexer/TokenStream.h"
#include "../CHTLNode/BaseNode.h"
#include "../../Scanner/StateMachine.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// Forward declarations
class CHTLParser;

// Parser state handlers
class ParserInitHandler : public StateHandler<ParserState> {
public:
    void onEnter(StateContext<ParserState>& context) override;
    void onUpdate(StateContext<ParserState>& context) override;
};

class ParserUseHandler : public StateHandler<ParserState> {
public:
    void onEnter(StateContext<ParserState>& context) override;
    void onUpdate(StateContext<ParserState>& context) override;
};

class ParserElementsHandler : public StateHandler<ParserState> {
public:
    void onEnter(StateContext<ParserState>& context) override;
    void onUpdate(StateContext<ParserState>& context) override;
};

class ParserCompleteHandler : public StateHandler<ParserState> {
public:
    void onEnter(StateContext<ParserState>& context) override;
    void onUpdate(StateContext<ParserState>& context) override;
};

class ParserErrorHandler : public StateHandler<ParserState> {
public:
    void onEnter(StateContext<ParserState>& context) override;
    void onUpdate(StateContext<ParserState>& context) override;
};

// Main CHTL Parser class
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;
    
    // Main parsing methods
    std::unique_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    std::unique_ptr<BaseNode> parse(const std::string& sourceCode);
    
    // Strategy management
    void registerStrategy(std::unique_ptr<ParserStrategy> strategy);
    void setDefaultStrategy(const std::string& name);
    
    // Error handling
    bool hasError() const { return !errors_.empty(); }
    std::vector<ParserError> getErrors() const { return errors_; }
    std::string getErrorSummary() const;
    void clearErrors() { errors_.clear(); }
    
    // Configuration
    void setStrictMode(bool strict) { strictMode_ = strict; }
    void setDebugMode(bool debug) { debugMode_ = debug; }
    
    // Parsing statistics
    size_t getParsedElementCount() const { return parsedElementCount_; }
    size_t getParsedTemplateCount() const { return parsedTemplateCount_; }
    size_t getParsedCustomCount() const { return parsedCustomCount_; }
    
    // Context management
    void setGlobalContext(const std::string& key, const std::string& value);
    std::string getGlobalContext(const std::string& key) const;
    bool hasGlobalContext(const std::string& key) const;
    void clearGlobalContext();
    
private:
    StateMachine<ParserState> stateMachine_;
    std::map<std::string, std::unique_ptr<ParserStrategy>> strategies_;
    
    // Parsing state
    std::unique_ptr<TokenStream> tokenStream_;
    std::unique_ptr<BaseNode> rootNode_;
    std::vector<ParserError> errors_;
    
    // Configuration
    bool strictMode_;
    bool debugMode_;
    
    // Statistics
    size_t parsedElementCount_;
    size_t parsedTemplateCount_;
    size_t parsedCustomCount_;
    
    // Global context
    std::map<std::string, std::string> globalContext_;
    
    // Strategy instances
    std::unique_ptr<UseStatementParser> useStatementParser_;
    std::unique_ptr<ElementParser> elementParser_;
    std::unique_ptr<AttributeParser> attributeParser_;
    std::unique_ptr<TemplateParser> templateParser_;
    std::unique_ptr<CustomParser> customParser_;
    std::unique_ptr<ImportParser> importParser_;
    std::unique_ptr<NamespaceParser> namespaceParser_;
    std::unique_ptr<OriginParser> originParser_;
    
    // Initialization
    void initializeStrategies();
    void initializeStateMachine();
    
    // Parsing methods
    bool parseUseStatement();
    bool parseElements();
    bool parseElement();
    bool parseTemplate();
    bool parseCustom();
    bool parseImport();
    bool parseNamespace();
    bool parseOrigin();
    
    // Utility methods
    ParserStrategy* selectStrategy(const Token& currentToken);
    void addError(ParserErrorType type, const std::string& message, size_t line = 0, size_t column = 0, const std::string& context = "");
    void logDebug(const std::string& message) const;
    
    // Token stream management
    bool consumeToken(TokenType expectedType, const std::string& description = "");
    bool consumeToken(const std::string& expectedValue, const std::string& description = "");
    bool peekToken(TokenType expectedType) const;
    bool peekToken(const std::string& expectedValue) const;
    
    // Node management
    void addNode(std::unique_ptr<BaseNode> node);
    BaseNode* getCurrentNode() const;
    void setCurrentNode(BaseNode* node);
    
    // Validation
    bool validateSyntax() const;
    bool validateSemantics() const;
    std::vector<std::string> getValidationErrors() const;
};

// Parser factory for creating parser instances
class ParserFactory {
public:
    static std::unique_ptr<CHTLParser> createParser();
    static std::unique_ptr<CHTLParser> createStrictParser();
    static std::unique_ptr<CHTLParser> createDebugParser();
};

// Parser utilities
class ParserUtils {
public:
    // Token utilities
    static bool isElementStart(const Token& token);
    static bool isTemplateStart(const Token& token);
    static bool isCustomStart(const Token& token);
    static bool isImportStart(const Token& token);
    static bool isNamespaceStart(const Token& token);
    static bool isOriginStart(const Token& token);
    
    // Error utilities
    static std::string formatError(const ParserError& error);
    static std::string formatErrorSummary(const std::vector<ParserError>& errors);
    
    // Validation utilities
    static bool isValidElementName(const std::string& name);
    static bool isValidTemplateName(const std::string& name);
    static bool isValidCustomName(const std::string& name);
    static bool isValidAttributeName(const std::string& name);
    static bool isValidAttributeValue(const std::string& value);
};

} // namespace CHTL