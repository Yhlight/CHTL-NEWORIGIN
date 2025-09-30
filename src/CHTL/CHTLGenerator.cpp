#include "CHTL/CHTLGenerator.h"
#include <sstream>
#include <fstream>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : isSuccessful_(false)
    , inlineMode_(false)
    , defaultStruct_(false) {
}

void CHTLGenerator::initialize() {
    isSuccessful_ = false;
    errorMessage_.clear();
    htmlOutput_.clear();
    cssOutput_.clear();
    jsOutput_.clear();
    outputOptions_.clear();
    templates_.clear();
    customElements_.clear();
    customStyles_.clear();
    customVars_.clear();
    imports_.clear();
    importPaths_.clear();
    namespaces_.clear();
    currentNamespace_.clear();
    configuration_.clear();
}

bool CHTLGenerator::generate(std::shared_ptr<BaseNode> astRoot) {
    try {
        if (!astRoot) {
            reportError("AST root is null");
            return false;
        }
        
        // Generate output from AST
        generateFromNode(astRoot);
        
        isSuccessful_ = true;
        return true;
        
    } catch (const std::exception& e) {
        reportError("Generation error: " + std::string(e.what()));
        return false;
    }
}

std::string CHTLGenerator::getAllOutput() const {
    std::stringstream ss;
    ss << "<!-- HTML Output -->\n";
    ss << htmlOutput_ << "\n\n";
    ss << "<!-- CSS Output -->\n";
    ss << "<style>\n" << cssOutput_ << "\n</style>\n\n";
    ss << "<!-- JavaScript Output -->\n";
    ss << "<script>\n" << jsOutput_ << "\n</script>";
    return ss.str();
}

void CHTLGenerator::reset() {
    initialize();
}

void CHTLGenerator::setOutputOptions(const std::unordered_map<std::string, std::string>& options) {
    outputOptions_ = options;
}

void CHTLGenerator::generateFromNode(std::shared_ptr<BaseNode> node) {
    if (!node) {
        return;
    }
    
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT:
            generateElement(node);
            break;
        case BaseNode::NodeType::TEXT:
            generateText(node);
            break;
        case BaseNode::NodeType::COMMENT:
            generateComment(node);
            break;
        case BaseNode::NodeType::STYLE:
            generateStyle(node);
            break;
        case BaseNode::NodeType::SCRIPT:
            generateScript(node);
            break;
        case BaseNode::NodeType::TEMPLATE:
            generateTemplate(node);
            break;
        case BaseNode::NodeType::CUSTOM:
            generateCustom(node);
            break;
        case BaseNode::NodeType::ORIGIN:
            generateOrigin(node);
            break;
        case BaseNode::NodeType::IMPORT:
            generateImport(node);
            break;
        case BaseNode::NodeType::CONFIG:
            generateConfig(node);
            break;
        case BaseNode::NodeType::NAMESPACE:
            generateNamespace(node);
            break;
        case BaseNode::NodeType::OPERATOR:
            generateOperator(node);
            break;
        case BaseNode::NodeType::ROOT:
            // Process children
            for (const auto& child : node->getChildren()) {
                generateFromNode(child);
            }
            break;
        default:
            break;
    }
}

void CHTLGenerator::generateElement(std::shared_ptr<BaseNode> element) {
    std::string html = generateHtmlElement(element);
    htmlOutput_ += html;
}

void CHTLGenerator::generateText(std::shared_ptr<BaseNode> text) {
    htmlOutput_ += escapeHtml(text->getContent());
}

void CHTLGenerator::generateComment(std::shared_ptr<BaseNode> comment) {
    htmlOutput_ += "<!-- " + comment->getContent() + " -->";
}

void CHTLGenerator::generateStyle(std::shared_ptr<BaseNode> style) {
    std::string css = generateCssRules(style);
    cssOutput_ += css;
}

void CHTLGenerator::generateScript(std::shared_ptr<BaseNode> script) {
    std::string js = generateJsCode(script);
    jsOutput_ += js;
}

void CHTLGenerator::generateTemplate(std::shared_ptr<BaseNode> templateNode) {
    // Store template for later use
    if (!templateNode->getName().empty()) {
        templates_[templateNode->getName()] = templateNode;
    }
}

void CHTLGenerator::generateCustom(std::shared_ptr<BaseNode> custom) {
    // Store custom element for later use
    if (!custom->getName().empty()) {
        customElements_[custom->getName()] = custom;
    }
}

void CHTLGenerator::generateOrigin(std::shared_ptr<BaseNode> origin) {
    // Output origin content directly
    htmlOutput_ += origin->getContent();
}

void CHTLGenerator::generateImport(std::shared_ptr<BaseNode> import) {
    // Handle import resolution
    resolveImport(import);
}

void CHTLGenerator::generateConfig(std::shared_ptr<BaseNode> config) {
    // Handle configuration
    handleConfiguration(config);
}

void CHTLGenerator::generateNamespace(std::shared_ptr<BaseNode> namespaceNode) {
    // Handle namespace
    handleNamespaces(namespaceNode);
}

void CHTLGenerator::generateOperator(std::shared_ptr<BaseNode> operatorNode) {
    // Handle operator
    // This is a stub implementation
}

std::string CHTLGenerator::generateHtmlElement(std::shared_ptr<BaseNode> element) {
    std::stringstream ss;
    
    if (element->getName().empty()) {
        return "";
    }
    
    ss << "<" << element->getName();
    
    // Add attributes
    std::string attrs = generateHtmlAttributes(element);
    if (!attrs.empty()) {
        ss << " " << attrs;
    }
    
    ss << ">";
    
    // Add content
    std::string content = generateHtmlContent(element);
    ss << content;
    
    // Add children
    for (const auto& child : element->getChildren()) {
        generateFromNode(child);
    }
    
    ss << "</" << element->getName() << ">";
    
    return ss.str();
}

std::string CHTLGenerator::generateHtmlAttributes(std::shared_ptr<BaseNode> element) {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& attr : element->getAttributes()) {
        if (!first) {
            ss << " ";
        }
        ss << attr.first << "=\"" << escapeHtml(attr.second) << "\"";
        first = false;
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateHtmlContent(std::shared_ptr<BaseNode> element) {
    return escapeHtml(element->getContent());
}

std::string CHTLGenerator::generateCssRules(std::shared_ptr<BaseNode> style) {
    std::stringstream ss;
    
    // Generate CSS rules from style node
    ss << "/* Generated CSS */\n";
    
    return ss.str();
}

std::string CHTLGenerator::generateCssSelector(std::shared_ptr<BaseNode> style) {
    return "." + style->getName();
}

std::string CHTLGenerator::generateCssProperties(std::shared_ptr<BaseNode> style) {
    std::stringstream ss;
    
    for (const auto& attr : style->getAttributes()) {
        ss << "  " << attr.first << ": " << generateCssValue(attr.second) << ";\n";
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateCssValue(const std::string& value) {
    return escapeCss(value);
}

std::string CHTLGenerator::generateJsCode(std::shared_ptr<BaseNode> script) {
    std::stringstream ss;
    
    // Generate JavaScript code from script node
    ss << "// Generated JavaScript\n";
    ss << script->getContent();
    
    return ss.str();
}

std::string CHTLGenerator::generateJsFunction(std::shared_ptr<BaseNode> script) {
    return "function " + script->getName() + "() {\n" + script->getContent() + "\n}";
}

std::string CHTLGenerator::generateJsExpression(const std::string& expression) {
    return escapeJs(expression);
}

void CHTLGenerator::expandTemplates(std::shared_ptr<BaseNode> node) {
    // Template expansion logic
}

void CHTLGenerator::expandTemplate(std::shared_ptr<BaseNode> node, const std::string& templateName) {
    auto it = templates_.find(templateName);
    if (it != templates_.end()) {
        generateFromNode(it->second);
    }
}

void CHTLGenerator::expandCustomElement(std::shared_ptr<BaseNode> node, const std::string& elementName) {
    auto it = customElements_.find(elementName);
    if (it != customElements_.end()) {
        generateFromNode(it->second);
    }
}

void CHTLGenerator::resolveImports(std::shared_ptr<BaseNode> node) {
    // Import resolution logic
}

void CHTLGenerator::resolveImport(std::shared_ptr<BaseNode> import) {
    // Import resolution logic
}

std::string CHTLGenerator::loadImportFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void CHTLGenerator::handleNamespaces(std::shared_ptr<BaseNode> node) {
    // Namespace handling logic
}

void CHTLGenerator::setNamespace(const std::string& namespaceName) {
    currentNamespace_ = namespaceName;
}

std::string CHTLGenerator::getNamespacedName(const std::string& name) {
    if (currentNamespace_.empty()) {
        return name;
    }
    return currentNamespace_ + "::" + name;
}

void CHTLGenerator::handleConfiguration(std::shared_ptr<BaseNode> config) {
    // Configuration handling logic
}

void CHTLGenerator::applyConfiguration() {
    // Apply configuration settings
}

void CHTLGenerator::processStyleBlocks(std::shared_ptr<BaseNode> node) {
    // Style processing logic
}

void CHTLGenerator::processLocalStyle(std::shared_ptr<BaseNode> style) {
    // Local style processing
}

void CHTLGenerator::processGlobalStyle(std::shared_ptr<BaseNode> style) {
    // Global style processing
}

void CHTLGenerator::processStyleOperations(std::shared_ptr<BaseNode> style) {
    // Style operations processing
}

void CHTLGenerator::processStyleConditions(std::shared_ptr<BaseNode> style) {
    // Style conditions processing
}

void CHTLGenerator::processStyleReferences(std::shared_ptr<BaseNode> style) {
    // Style references processing
}

void CHTLGenerator::processScriptBlocks(std::shared_ptr<BaseNode> node) {
    // Script processing logic
}

void CHTLGenerator::processLocalScript(std::shared_ptr<BaseNode> script) {
    // Local script processing
}

void CHTLGenerator::processGlobalScript(std::shared_ptr<BaseNode> script) {
    // Global script processing
}

void CHTLGenerator::processChtlJsSyntax(std::shared_ptr<BaseNode> script) {
    // CHTL JS syntax processing
}

std::string CHTLGenerator::escapeHtml(const std::string& text) const {
    std::string result = text;
    
    // Replace & first to avoid double escaping
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string CHTLGenerator::escapeCss(const std::string& text) const {
    std::string result = text;
    
    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

std::string CHTLGenerator::escapeJs(const std::string& text) const {
    std::string result = text;
    
    size_t pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\n", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\r", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\r");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\t", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\t");
        pos += 2;
    }
    
    return result;
}

std::string CHTLGenerator::indent(size_t level) const {
    return std::string(level * 2, ' ');
}

std::string CHTLGenerator::formatOutput(const std::string& output) const {
    return output; // Basic formatting
}

void CHTLGenerator::reportError(const std::string& message) {
    errorMessage_ = message;
    isSuccessful_ = false;
}

void CHTLGenerator::reportError(const std::string& message, std::shared_ptr<BaseNode> node) {
    errorMessage_ = message + " at node: " + node->getName();
    isSuccessful_ = false;
}

} // namespace CHTL