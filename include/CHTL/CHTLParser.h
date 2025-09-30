#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include "CHTLLexer.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLState.h"
#include "CHTLStrategy.h"

namespace CHTL {

/**
 * @brief CHTL Parser - Builds AST from tokens
 * 
 * Uses recursive descent parsing with state machine and strategy pattern
 * to handle all CHTL syntax features including complex nested structures
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;
    
    /**
     * @brief Initialize parser
     */
    void initialize();
    
    /**
     * @brief Parse tokens into AST
     * @param tokens Vector of tokens to parse
     * @return Root node of AST
     */
    std::shared_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    
    /**
     * @brief Parse file into AST
     * @param filePath Path to file to parse
     * @return Root node of AST
     */
    std::shared_ptr<BaseNode> parseFile(const std::string& filePath);
    
    /**
     * @brief Parse string into AST
     * @param input Input string to parse
     * @return Root node of AST
     */
    std::shared_ptr<BaseNode> parseString(const std::string& input);
    
    /**
     * @brief Get current AST root
     * @return Current AST root
     */
    std::shared_ptr<BaseNode> getAstRoot() const { return astRoot_; }
    
    /**
     * @brief Check if parsing was successful
     * @return True if successful, false if error
     */
    bool isSuccessful() const { return isSuccessful_; }
    
    /**
     * @brief Get error message
     * @return Error message
     */
    std::string getErrorMessage() const { return errorMessage_; }
    
    /**
     * @brief Reset parser state
     */
    void reset();

private:
    // Parser state
    std::vector<Token> tokens_;
    size_t currentTokenIndex_;
    std::shared_ptr<BaseNode> astRoot_;
    bool isSuccessful_;
    std::string errorMessage_;
    
    // State machine and strategy pattern
    std::unique_ptr<CHTLStateManager> stateManager_;
    std::unique_ptr<CHTLStrategyManager> strategyManager_;
    
    // Parser stack for nested structures
    std::stack<std::shared_ptr<BaseNode>> nodeStack_;
    
    // Parsing methods
    void parseTokens();
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseTextNode();
    std::shared_ptr<BaseNode> parseComment();
    std::shared_ptr<BaseNode> parseStyleBlock();
    std::shared_ptr<BaseNode> parseScriptBlock();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseConfig();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseOperator();
    
    // Attribute parsing
    void parseAttributes(std::shared_ptr<BaseNode> node);
    std::pair<std::string, std::string> parseAttribute();
    
    // Style parsing
    void parseStyleContent(std::shared_ptr<BaseNode> styleNode);
    void parseStyleProperty(std::shared_ptr<BaseNode> styleNode);
    void parseStyleSelector(std::shared_ptr<BaseNode> styleNode);
    void parseStyleValue(std::shared_ptr<BaseNode> styleNode);
    
    // Template parsing
    void parseTemplateContent(std::shared_ptr<BaseNode> templateNode);
    void parseTemplateInheritance(std::shared_ptr<BaseNode> templateNode);
    
    // Custom parsing
    void parseCustomContent(std::shared_ptr<BaseNode> customNode);
    void parseCustomSpecialization(std::shared_ptr<BaseNode> customNode);
    
    // Import parsing
    void parseImportContent(std::shared_ptr<BaseNode> importNode);
    
    // Configuration parsing
    void parseConfigContent(std::shared_ptr<BaseNode> configNode);
    
    // Namespace parsing
    void parseNamespaceContent(std::shared_ptr<BaseNode> namespaceNode);
    
    // Operator parsing
    void parseOperatorContent(std::shared_ptr<BaseNode> operatorNode);
    
    // Token management
    const Token& getCurrentToken() const;
    const Token& peekToken() const;
    const Token& peekToken(size_t offset) const;
    bool advanceToken();
    bool matchToken(TokenType type);
    bool matchToken(TokenType type, const std::string& value);
    bool isAtEnd() const;
    
    // Error handling
    void reportError(const std::string& message);
    void reportError(const std::string& message, const Token& token);
    bool hasError() const;
    
    // Helper methods
    bool isElementToken(const Token& token) const;
    bool isKeywordToken(const Token& token) const;
    bool isOperatorToken(const Token& token) const;
    bool isSpecialToken(const Token& token) const;
    
    // Validation
    bool validateNode(std::shared_ptr<BaseNode> node) const;
    bool validateElement(std::shared_ptr<BaseNode> element) const;
    bool validateStyle(std::shared_ptr<BaseNode> style) const;
    bool validateScript(std::shared_ptr<BaseNode> script) const;
    bool validateTemplate(std::shared_ptr<BaseNode> templateNode) const;
    bool validateCustom(std::shared_ptr<BaseNode> custom) const;
};

} // namespace CHTL