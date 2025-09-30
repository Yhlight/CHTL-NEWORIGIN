#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace CHTL {

CHTLContext::CHTLContext() 
    : compiled_(false), currentNamespace_("") {
    initializeStrategies();
}

void CHTLContext::initializeStrategies() {
    // Register all parsing strategies
    strategyManager_.registerStrategy(std::make_unique<ElementStrategy>());
    strategyManager_.registerStrategy(std::make_unique<AttributeStrategy>());
    strategyManager_.registerStrategy(std::make_unique<TextNodeStrategy>());
    strategyManager_.registerStrategy(std::make_unique<CommentStrategy>());
    strategyManager_.registerStrategy(std::make_unique<StyleBlockStrategy>());
    strategyManager_.registerStrategy(std::make_unique<ScriptBlockStrategy>());
    strategyManager_.registerStrategy(std::make_unique<TemplateStrategy>());
    strategyManager_.registerStrategy(std::make_unique<CustomStrategy>());
    strategyManager_.registerStrategy(std::make_unique<ImportStrategy>());
    strategyManager_.registerStrategy(std::make_unique<NamespaceStrategy>());
    strategyManager_.registerStrategy(std::make_unique<ConfigurationStrategy>());
    strategyManager_.registerStrategy(std::make_unique<OriginBlockStrategy>());
}

bool CHTLContext::compile(const std::string& source) {
    reset();
    setSource(source);
    
    // Lexical analysis
    CHTL::Lexer lexer;
    tokens_ = lexer.tokenize(source);
    
    if (lexer.hasError()) {
        addError("Lexical error: " + lexer.getErrorMessage());
        return false;
    }
    
    // Create root node
    rootNode_ = std::make_unique<RootNode>();
    
    // Parse tokens
    if (!processTokens()) {
        return false;
    }
    
    compiled_ = true;
    return true;
}

bool CHTLContext::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Could not open file: " + filePath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return compile(buffer.str());
}

std::string CHTLContext::generateHTML() const {
    if (!rootNode_) {
        return "";
    }
    return rootNode_->generateHTML();
}

std::string CHTLContext::generateCSS() const {
    if (!rootNode_) {
        return "";
    }
    return rootNode_->generateCSS();
}

std::string CHTLContext::generateJS() const {
    if (!rootNode_) {
        return "";
    }
    return rootNode_->generateJS();
}

std::string CHTLContext::generateAll() const {
    std::ostringstream oss;
    
    std::string html = generateHTML();
    std::string css = generateCSS();
    std::string js = generateJS();
    
    if (!html.empty()) {
        oss << "<!-- HTML -->\n" << html << "\n\n";
    }
    
    if (!css.empty()) {
        oss << "<!-- CSS -->\n<style>\n" << css << "\n</style>\n\n";
    }
    
    if (!js.empty()) {
        oss << "<!-- JavaScript -->\n<script>\n" << js << "\n</script>\n";
    }
    
    return oss.str();
}

void CHTLContext::reset() {
    source_.clear();
    tokens_.clear();
    rootNode_.reset();
    stateManager_.resetToInitial();
    compiled_ = false;
    errors_.clear();
    configuration_.clear();
    currentNamespace_.clear();
    namespaceStack_.clear();
    templates_.clear();
    customs_.clear();
    imports_.clear();
}

void CHTLContext::setSource(const std::string& source) {
    source_ = source;
}

void CHTLContext::setRootNode(std::unique_ptr<BaseNode> root) {
    rootNode_ = std::move(root);
}

void CHTLContext::setTokens(const std::vector<Token>& tokens) {
    tokens_ = tokens;
}

TokenStream CHTLContext::getTokenStream() const {
    return TokenStream(tokens_);
}

bool CHTLContext::hasError() const {
    return !errors_.empty() || stateManager_.hasError();
}

const std::string& CHTLContext::getErrorMessage() const {
    if (!errors_.empty()) {
        return errors_.back();
    }
    return stateManager_.getErrorMessage();
}

std::vector<std::string> CHTLContext::getAllErrors() const {
    std::vector<std::string> allErrors = errors_;
    
    if (stateManager_.hasError()) {
        allErrors.push_back(stateManager_.getErrorMessage());
    }
    
    return allErrors;
}

void CHTLContext::addError(const std::string& error) {
    errors_.push_back(error);
}

void CHTLContext::clearErrors() {
    errors_.clear();
}

void CHTLContext::setConfiguration(const std::string& key, const std::any& value) {
    configuration_[key] = value;
}

std::any CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configuration_.find(key);
    return it != configuration_.end() ? it->second : std::any{};
}

bool CHTLContext::hasConfiguration(const std::string& key) const {
    return configuration_.find(key) != configuration_.end();
}

void CHTLContext::enterNamespace(const std::string& name) {
    namespaceStack_.push_back(currentNamespace_);
    currentNamespace_ = name;
}

void CHTLContext::exitNamespace() {
    if (!namespaceStack_.empty()) {
        currentNamespace_ = namespaceStack_.back();
        namespaceStack_.pop_back();
    } else {
        currentNamespace_.clear();
    }
}

void CHTLContext::registerTemplate(const std::string& name, std::unique_ptr<BaseNode> templateNode) {
    templates_[name] = std::move(templateNode);
}

BaseNode* CHTLContext::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return it != templates_.end() ? it->second.get() : nullptr;
}

bool CHTLContext::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

void CHTLContext::registerCustom(const std::string& name, std::unique_ptr<BaseNode> customNode) {
    customs_[name] = std::move(customNode);
}

BaseNode* CHTLContext::getCustom(const std::string& name) const {
    auto it = customs_.find(name);
    return it != customs_.end() ? it->second.get() : nullptr;
}

bool CHTLContext::hasCustom(const std::string& name) const {
    return customs_.find(name) != customs_.end();
}

void CHTLContext::addImport(const std::string& path, const std::string& alias) {
    imports_.emplace_back(path, alias);
}

std::string CHTLContext::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLContext Debug Info:\n";
    oss << "  Compiled: " << (compiled_ ? "Yes" : "No") << "\n";
    oss << "  Source length: " << source_.length() << " characters\n";
    oss << "  Token count: " << tokens_.size() << "\n";
    oss << "  Current state: " << static_cast<int>(stateManager_.getCurrentState()->getType()) << "\n";
    oss << "  Current namespace: " << (currentNamespace_.empty() ? "None" : currentNamespace_) << "\n";
    oss << "  Templates: " << templates_.size() << "\n";
    oss << "  Customs: " << customs_.size() << "\n";
    oss << "  Imports: " << imports_.size() << "\n";
    oss << "  Errors: " << errors_.size() << "\n";
    return oss.str();
}

std::string CHTLContext::getASTString() const {
    if (!rootNode_) {
        return "No AST available";
    }
    return rootNode_->toDebugString();
}

std::string CHTLContext::getTokenString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < tokens_.size(); ++i) {
        oss << "[" << i << "] " << tokens_[i].toString() << "\n";
    }
    return oss.str();
}

bool CHTLContext::processTokens() {
    TokenStream stream = getTokenStream();
    
    while (stream.hasNext()) {
        const Token& token = stream.current();
        
        if (!processToken(token)) {
            addError("Failed to process token: " + token.toString());
            return false;
        }
        
        stream.advance();
    }
    
    return true;
}

bool CHTLContext::processToken(const Token& token) {
    CHTLState* currentState = stateManager_.getCurrentState();
    if (!currentState) {
        addError("No current state available");
        return false;
    }
    
    CHTLStrategy* strategy = strategyManager_.findStrategy(token, currentState->getType());
    if (!strategy) {
        addError("No strategy found for token: " + token.toString());
        return false;
    }
    
    return strategy->execute(*this, token);
}

void CHTLContext::handleError(const std::string& error) {
    addError(error);
}

} // namespace CHTL