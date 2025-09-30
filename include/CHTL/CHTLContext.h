#pragma once

#include "CHTL/CHTLState.h"
#include "CHTL/CHTLStrategy.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief Main context class for CHTL compilation
 * 
 * This class coordinates the entire CHTL compilation process using
 * the state machine + strategy pattern architecture.
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;

    // Main compilation interface
    bool compile(const std::string& source);
    bool compileFile(const std::string& filePath);
    
    // Output generation
    std::string generateHTML() const;
    std::string generateCSS() const;
    std::string generateJS() const;
    std::string generateAll() const;

    // Context management
    void reset();
    void setSource(const std::string& source);
    const std::string& getSource() const { return source_; }

    // State management
    CHTLStateManager& getStateManager() { return stateManager_; }
    const CHTLStateManager& getStateManager() const { return stateManager_; }

    // Strategy management
    CHTLStrategyManager& getStrategyManager() { return strategyManager_; }
    const CHTLStrategyManager& getStrategyManager() const { return strategyManager_; }

    // AST management
    void setRootNode(std::unique_ptr<BaseNode> root);
    BaseNode* getRootNode() const { return rootNode_.get(); }
    const std::unique_ptr<BaseNode>& getRootNodePtr() const { return rootNode_; }

    // Token management
    void setTokens(const std::vector<Token>& tokens);
    const std::vector<Token>& getTokens() const { return tokens_; }
    TokenStream getTokenStream() const;

    // Error handling
    bool hasError() const;
    const std::string& getErrorMessage() const;
    std::vector<std::string> getAllErrors() const;
    void addError(const std::string& error);
    void clearErrors();

    // Compilation status
    bool isCompiled() const { return compiled_; }
    void setCompiled(bool compiled) { compiled_ = compiled; }

    // Configuration
    void setConfiguration(const std::string& key, const std::any& value);
    std::any getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;

    // Namespace management
    void enterNamespace(const std::string& name);
    void exitNamespace();
    const std::string& getCurrentNamespace() const { return currentNamespace_; }
    bool isInNamespace() const { return !currentNamespace_.empty(); }

    // Template management
    void registerTemplate(const std::string& name, std::unique_ptr<BaseNode> templateNode);
    BaseNode* getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;

    // Custom definition management
    void registerCustom(const std::string& name, std::unique_ptr<BaseNode> customNode);
    BaseNode* getCustom(const std::string& name) const;
    bool hasCustom(const std::string& name) const;

    // Import management
    void addImport(const std::string& path, const std::string& alias = "");
    const std::vector<std::pair<std::string, std::string>>& getImports() const { return imports_; }

    // Debugging and diagnostics
    std::string getDebugInfo() const;
    std::string getASTString() const;
    std::string getTokenString() const;

private:
    // Core components
    std::string source_;
    std::vector<Token> tokens_;
    std::unique_ptr<BaseNode> rootNode_;
    CHTLStateManager stateManager_;
    CHTLStrategyManager strategyManager_;

    // Compilation state
    bool compiled_;
    std::vector<std::string> errors_;

    // Configuration
    std::unordered_map<std::string, std::any> configuration_;

    // Namespace management
    std::string currentNamespace_;
    std::vector<std::string> namespaceStack_;

    // Template and custom definitions
    std::unordered_map<std::string, std::unique_ptr<BaseNode>> templates_;
    std::unordered_map<std::string, std::unique_ptr<BaseNode>> customs_;

    // Import management
    std::vector<std::pair<std::string, std::string>> imports_;

    // Internal methods
    void initializeStrategies();
    bool processTokens();
    bool processToken(const Token& token);
    void handleError(const std::string& error);
};

} // namespace CHTL