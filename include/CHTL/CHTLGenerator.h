#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "CHTLNode/BaseNode.h"

namespace CHTL {

/**
 * @brief CHTL Generator - Generates HTML/CSS/JS from AST
 * 
 * Handles all CHTL output generation including:
 * - HTML structure generation
 * - CSS generation (global and local styles)
 * - JavaScript generation
 * - Template expansion
 * - Custom element expansion
 * - Import resolution
 * - Namespace handling
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;
    
    /**
     * @brief Initialize generator
     */
    void initialize();
    
    /**
     * @brief Generate output from AST
     * @param astRoot Root node of AST
     * @return True if successful, false if error
     */
    bool generate(std::shared_ptr<BaseNode> astRoot);
    
    /**
     * @brief Get generated HTML
     * @return Generated HTML
     */
    std::string getHtmlOutput() const { return htmlOutput_; }
    
    /**
     * @brief Get generated CSS
     * @return Generated CSS
     */
    std::string getCssOutput() const { return cssOutput_; }
    
    /**
     * @brief Get generated JavaScript
     * @return Generated JavaScript
     */
    std::string getJsOutput() const { return jsOutput_; }
    
    /**
     * @brief Get all output combined
     * @return Combined output
     */
    std::string getAllOutput() const;
    
    /**
     * @brief Check if generation was successful
     * @return True if successful, false if error
     */
    bool isSuccessful() const { return isSuccessful_; }
    
    /**
     * @brief Get error message
     * @return Error message
     */
    std::string getErrorMessage() const { return errorMessage_; }
    
    /**
     * @brief Reset generator state
     */
    void reset();
    
    /**
     * @brief Set output options
     * @param options Output options
     */
    void setOutputOptions(const std::unordered_map<std::string, std::string>& options);
    
    /**
     * @brief Set inline mode
     * @param inlineMode True for inline output, false for separate files
     */
    void setInlineMode(bool inlineMode) { inlineMode_ = inlineMode; }
    
    /**
     * @brief Set default structure mode
     * @param defaultStruct True to include default HTML structure
     */
    void setDefaultStructMode(bool defaultStruct) { defaultStruct_ = defaultStruct; }

private:
    // Output strings
    std::string htmlOutput_;
    std::string cssOutput_;
    std::string jsOutput_;
    
    // Generation state
    bool isSuccessful_;
    std::string errorMessage_;
    bool inlineMode_;
    bool defaultStruct_;
    
    // Output options
    std::unordered_map<std::string, std::string> outputOptions_;
    
    // Template and custom element storage
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> templates_;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> customElements_;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> customStyles_;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> customVars_;
    
    // Import resolution
    std::unordered_map<std::string, std::string> imports_;
    std::vector<std::string> importPaths_;
    
    // Namespace management
    std::unordered_map<std::string, std::string> namespaces_;
    std::string currentNamespace_;
    
    // Configuration
    std::unordered_map<std::string, std::string> configuration_;
    
    // Generation methods
    void generateFromNode(std::shared_ptr<BaseNode> node);
    void generateElement(std::shared_ptr<BaseNode> element);
    void generateText(std::shared_ptr<BaseNode> text);
    void generateComment(std::shared_ptr<BaseNode> comment);
    void generateStyle(std::shared_ptr<BaseNode> style);
    void generateScript(std::shared_ptr<BaseNode> script);
    void generateTemplate(std::shared_ptr<BaseNode> templateNode);
    void generateCustom(std::shared_ptr<BaseNode> custom);
    void generateOrigin(std::shared_ptr<BaseNode> origin);
    void generateImport(std::shared_ptr<BaseNode> import);
    void generateConfig(std::shared_ptr<BaseNode> config);
    void generateNamespace(std::shared_ptr<BaseNode> namespaceNode);
    void generateOperator(std::shared_ptr<BaseNode> operatorNode);
    
    // HTML generation
    std::string generateHtmlElement(std::shared_ptr<BaseNode> element);
    std::string generateHtmlAttributes(std::shared_ptr<BaseNode> element);
    std::string generateHtmlContent(std::shared_ptr<BaseNode> element);
    
    // CSS generation
    std::string generateCssRules(std::shared_ptr<BaseNode> style);
    std::string generateCssSelector(std::shared_ptr<BaseNode> style);
    std::string generateCssProperties(std::shared_ptr<BaseNode> style);
    std::string generateCssValue(const std::string& value);
    
    // JavaScript generation
    std::string generateJsCode(std::shared_ptr<BaseNode> script);
    std::string generateJsFunction(std::shared_ptr<BaseNode> script);
    std::string generateJsExpression(const std::string& expression);
    
    // Template expansion
    void expandTemplates(std::shared_ptr<BaseNode> node);
    void expandTemplate(std::shared_ptr<BaseNode> node, const std::string& templateName);
    void expandCustomElement(std::shared_ptr<BaseNode> node, const std::string& elementName);
    
    // Import resolution
    void resolveImports(std::shared_ptr<BaseNode> node);
    void resolveImport(std::shared_ptr<BaseNode> import);
    std::string loadImportFile(const std::string& filePath);
    
    // Namespace handling
    void handleNamespaces(std::shared_ptr<BaseNode> node);
    void setNamespace(const std::string& namespaceName);
    std::string getNamespacedName(const std::string& name);
    
    // Configuration handling
    void handleConfiguration(std::shared_ptr<BaseNode> config);
    void applyConfiguration();
    
    // Style processing
    void processStyleBlocks(std::shared_ptr<BaseNode> node);
    void processLocalStyle(std::shared_ptr<BaseNode> style);
    void processGlobalStyle(std::shared_ptr<BaseNode> style);
    void processStyleOperations(std::shared_ptr<BaseNode> style);
    void processStyleConditions(std::shared_ptr<BaseNode> style);
    void processStyleReferences(std::shared_ptr<BaseNode> style);
    
    // Script processing
    void processScriptBlocks(std::shared_ptr<BaseNode> node);
    void processLocalScript(std::shared_ptr<BaseNode> script);
    void processGlobalScript(std::shared_ptr<BaseNode> script);
    void processChtlJsSyntax(std::shared_ptr<BaseNode> script);
    
    // Utility methods
    std::string escapeHtml(const std::string& text) const;
    std::string escapeCss(const std::string& text) const;
    std::string escapeJs(const std::string& text) const;
    std::string indent(size_t level) const;
    std::string formatOutput(const std::string& output) const;
    
    // Error handling
    void reportError(const std::string& message);
    void reportError(const std::string& message, std::shared_ptr<BaseNode> node);
};

} // namespace CHTL