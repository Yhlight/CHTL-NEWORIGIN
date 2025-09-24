#include "CHTLParser.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <sstream>

namespace CHTL {

// CHTLParser implementation
CHTLParser::CHTLParser() 
    : stateMachine_(ParserState::INIT)
    , strictMode_(false)
    , debugMode_(false)
    , parsedElementCount_(0)
    , parsedTemplateCount_(0)
    , parsedCustomCount_(0) {
    
    initializeStrategies();
    initializeStateMachine();
}

void CHTLParser::initializeStrategies() {
    // Create strategy instances
    useStatementParser_ = std::make_unique<UseStatementParser>();
    elementParser_ = std::make_unique<ElementParser>();
    attributeParser_ = std::make_unique<AttributeParser>();
    templateParser_ = std::make_unique<TemplateParser>();
    customParser_ = std::make_unique<CustomParser>();
    importParser_ = std::make_unique<ImportParser>();
    namespaceParser_ = std::make_unique<NamespaceParser>();
    originParser_ = std::make_unique<OriginParser>();
    
    // Register strategies
    registerStrategy(std::make_unique<UseStatementParser>());
    registerStrategy(std::make_unique<ElementParser>());
    registerStrategy(std::make_unique<AttributeParser>());
    registerStrategy(std::make_unique<TemplateParser>());
    registerStrategy(std::make_unique<CustomParser>());
    registerStrategy(std::make_unique<ImportParser>());
    registerStrategy(std::make_unique<NamespaceParser>());
    registerStrategy(std::make_unique<OriginParser>());
}

void CHTLParser::initializeStateMachine() {
    // Add state handlers
    stateMachine_.setHandler(ParserState::INIT, std::make_unique<ParserInitHandler>());
    stateMachine_.setHandler(ParserState::PARSE_USE, std::make_unique<ParserUseHandler>());
    stateMachine_.setHandler(ParserState::PARSE_ELEMENTS, std::make_unique<ParserElementsHandler>());
    stateMachine_.setHandler(ParserState::COMPLETE, std::make_unique<ParserCompleteHandler>());
    stateMachine_.setHandler(ParserState::ERROR, std::make_unique<ParserErrorHandler>());
    
    // Add transitions
    stateMachine_.addTransition(ParserState::INIT, ParserState::PARSE_USE, "start_parsing");
    stateMachine_.addTransition(ParserState::PARSE_USE, ParserState::PARSE_ELEMENTS, "use_parsed");
    stateMachine_.addTransition(ParserState::PARSE_ELEMENTS, ParserState::COMPLETE, "elements_parsed");
    
    // Error transitions
    stateMachine_.addTransition(ParserState::INIT, ParserState::ERROR, "error");
    stateMachine_.addTransition(ParserState::PARSE_USE, ParserState::ERROR, "error");
    stateMachine_.addTransition(ParserState::PARSE_ELEMENTS, ParserState::ERROR, "error");
}

std::unique_ptr<BaseNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    clearErrors();
    parsedElementCount_ = 0;
    parsedTemplateCount_ = 0;
    parsedCustomCount_ = 0;
    
    tokenStream_ = std::make_unique<TokenStream>(tokens);
    rootNode_ = std::make_unique<BaseNode>(NodeType::BASE);
    rootNode_->setProperty("type", "chtl_document");
    
    logDebug("Starting CHTL parsing with " + std::to_string(tokens.size()) + " tokens");
    
    try {
        // Start parsing with use statement
        if (!parseUseStatement()) {
            addError(ParserErrorType::SYNTAX_ERROR, "Failed to parse use statement");
            return nullptr;
        }
        
        // Parse elements
        if (!parseElements()) {
            addError(ParserErrorType::SYNTAX_ERROR, "Failed to parse elements");
            return nullptr;
        }
        
        // Validate parsing results
        if (!validateSyntax()) {
            return nullptr;
        }
        
        logDebug("CHTL parsing completed successfully");
        return std::move(rootNode_);
        
    } catch (const std::exception& e) {
        addError(ParserErrorType::SYNTAX_ERROR, "Parsing exception: " + std::string(e.what()));
        return nullptr;
    }
}

std::unique_ptr<BaseNode> CHTLParser::parse(const std::string& sourceCode) {
    Lexer lexer;
    auto tokens = lexer.tokenize(sourceCode);
    
    if (lexer.hasError()) {
        addError(ParserErrorType::SYNTAX_ERROR, "Lexical analysis failed: " + lexer.getError());
        return nullptr;
    }
    
    return parse(tokens);
}

void CHTLParser::registerStrategy(std::unique_ptr<ParserStrategy> strategy) {
    if (strategy) {
        strategies_[strategy->getName()] = std::move(strategy);
    }
}

void CHTLParser::setDefaultStrategy(const std::string& name) {
    // Implementation for setting default strategy
}

void CHTLParser::setGlobalContext(const std::string& key, const std::string& value) {
    globalContext_[key] = value;
}

std::string CHTLParser::getGlobalContext(const std::string& key) const {
    auto it = globalContext_.find(key);
    return (it != globalContext_.end()) ? it->second : "";
}

bool CHTLParser::hasGlobalContext(const std::string& key) const {
    return globalContext_.find(key) != globalContext_.end();
}

void CHTLParser::clearGlobalContext() {
    globalContext_.clear();
}

bool CHTLParser::parseUseStatement() {
    logDebug("Parsing use statement");
    
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        addError(ParserErrorType::SYNTAX_ERROR, "Expected 'use' statement");
        return false;
    }
    
    // Parse 'use html5 {' pattern
    if (!consumeToken(TokenType::KEYWORD_USE, "use keyword")) {
        return false;
    }
    
    if (!consumeToken(TokenType::KEYWORD_HTML5, "html5 keyword")) {
        return false;
    }
    
    if (!consumeToken(TokenType::BRACE_OPEN, "opening brace")) {
        return false;
    }
    
    logDebug("Use statement parsed successfully");
    return true;
}

bool CHTLParser::parseElements() {
    logDebug("Parsing elements");
    
    while (tokenStream_->hasMore()) {
        const Token& currentToken = tokenStream_->peek();
        
        // Check for closing brace (end of document)
        if (currentToken.getType() == TokenType::BRACE_CLOSE) {
            tokenStream_->consume();
            logDebug("End of document reached");
            break;
        }
        
        // Parse individual element
        if (!parseElement()) {
            // Try to parse other constructs
            if (!parseTemplate() && !parseCustom() && !parseImport() && !parseNamespace() && !parseOrigin()) {
                addError(ParserErrorType::SYNTAX_ERROR, "Unexpected token: " + currentToken.getValue());
                return false;
            }
        }
    }
    
    logDebug("Elements parsing completed");
    return true;
}

bool CHTLParser::parseElement() {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& currentToken = tokenStream_->peek();
    if (!ParserUtils::isElementStart(currentToken)) {
        return false;
    }
    
    ParserStrategy* strategy = selectStrategy(currentToken);
    if (!strategy || strategy->getName() != "ElementParser") {
        return false;
    }
    
    auto element = strategy->parse(*tokenStream_);
    if (element) {
        rootNode_->addChild(std::move(element));
        parsedElementCount_++;
        logDebug("Element parsed successfully");
        return true;
    }
    
    return false;
}

bool CHTLParser::parseTemplate() {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& currentToken = tokenStream_->peek();
    if (!ParserUtils::isTemplateStart(currentToken)) {
        return false;
    }
    
    ParserStrategy* strategy = selectStrategy(currentToken);
    if (!strategy || strategy->getName() != "TemplateParser") {
        return false;
    }
    
    auto template = strategy->parse(*tokenStream_);
    if (template) {
        rootNode_->addChild(std::move(template));
        parsedTemplateCount_++;
        logDebug("Template parsed successfully");
        return true;
    }
    
    return false;
}

bool CHTLParser::parseCustom() {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& currentToken = tokenStream_->peek();
    if (!ParserUtils::isCustomStart(currentToken)) {
        return false;
    }
    
    ParserStrategy* strategy = selectStrategy(currentToken);
    if (!strategy || strategy->getName() != "CustomParser") {
        return false;
    }
    
    auto custom = strategy->parse(*tokenStream_);
    if (custom) {
        rootNode_->addChild(std::move(custom));
        parsedCustomCount_++;
        logDebug("Custom component parsed successfully");
        return true;
    }
    
    return false;
}

bool CHTLParser::parseImport() {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& currentToken = tokenStream_->peek();
    if (!ParserUtils::isImportStart(currentToken)) {
        return false;
    }
    
    ParserStrategy* strategy = selectStrategy(currentToken);
    if (!strategy || strategy->getName() != "ImportParser") {
        return false;
    }
    
    auto import = strategy->parse(*tokenStream_);
    if (import) {
        rootNode_->addChild(std::move(import));
        logDebug("Import parsed successfully");
        return true;
    }
    
    return false;
}

bool CHTLParser::parseNamespace() {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& currentToken = tokenStream_->peek();
    if (!ParserUtils::isNamespaceStart(currentToken)) {
        return false;
    }
    
    ParserStrategy* strategy = selectStrategy(currentToken);
    if (!strategy || strategy->getName() != "NamespaceParser") {
        return false;
    }
    
    auto namespaceNode = strategy->parse(*tokenStream_);
    if (namespaceNode) {
        rootNode_->addChild(std::move(namespaceNode));
        logDebug("Namespace parsed successfully");
        return true;
    }
    
    return false;
}

bool CHTLParser::parseOrigin() {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& currentToken = tokenStream_->peek();
    if (!ParserUtils::isOriginStart(currentToken)) {
        return false;
    }
    
    ParserStrategy* strategy = selectStrategy(currentToken);
    if (!strategy || strategy->getName() != "OriginParser") {
        return false;
    }
    
    auto origin = strategy->parse(*tokenStream_);
    if (origin) {
        rootNode_->addChild(std::move(origin));
        logDebug("Origin parsed successfully");
        return true;
    }
    
    return false;
}

ParserStrategy* CHTLParser::selectStrategy(const Token& currentToken) {
    for (auto& [name, strategy] : strategies_) {
        if (strategy && strategy->canHandle(currentToken, *tokenStream_)) {
            return strategy.get();
        }
    }
    return nullptr;
}

void CHTLParser::addError(ParserErrorType type, const std::string& message, size_t line, size_t column, const std::string& context) {
    ParserError error(type, message, line, column, context);
    errors_.push_back(error);
    
    if (debugMode_) {
        std::cerr << "Parser Error: " << ParserUtils::formatError(error) << std::endl;
    }
}

void CHTLParser::logDebug(const std::string& message) const {
    if (debugMode_) {
        std::cout << "[CHTLParser] " << message << std::endl;
    }
}

bool CHTLParser::consumeToken(TokenType expectedType, const std::string& description) {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        addError(ParserErrorType::MISSING_TOKEN, "Expected " + description);
        return false;
    }
    
    const Token& token = tokenStream_->consume();
    if (token.getType() != expectedType) {
        addError(ParserErrorType::UNEXPECTED_TOKEN, 
                "Expected " + description + ", got: " + token.getValue(),
                token.getLine(), token.getColumn());
        return false;
    }
    
    return true;
}

bool CHTLParser::consumeToken(const std::string& expectedValue, const std::string& description) {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        addError(ParserErrorType::MISSING_TOKEN, "Expected " + description);
        return false;
    }
    
    const Token& token = tokenStream_->consume();
    if (token.getValue() != expectedValue) {
        addError(ParserErrorType::UNEXPECTED_TOKEN,
                "Expected " + description + ", got: " + token.getValue(),
                token.getLine(), token.getColumn());
        return false;
    }
    
    return true;
}

bool CHTLParser::peekToken(TokenType expectedType) const {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& token = tokenStream_->peek();
    return token.getType() == expectedType;
}

bool CHTLParser::peekToken(const std::string& expectedValue) const {
    if (!tokenStream_ || !tokenStream_->hasMore()) {
        return false;
    }
    
    const Token& token = tokenStream_->peek();
    return token.getValue() == expectedValue;
}

void CHTLParser::addNode(std::unique_ptr<BaseNode> node) {
    if (node && rootNode_) {
        rootNode_->addChild(std::move(node));
    }
}

BaseNode* CHTLParser::getCurrentNode() const {
    return rootNode_.get();
}

void CHTLParser::setCurrentNode(BaseNode* node) {
    // Implementation for setting current node
}

bool CHTLParser::validateSyntax() const {
    // Basic syntax validation
    return true; // TODO: Implement comprehensive syntax validation
}

bool CHTLParser::validateSemantics() const {
    // Semantic validation
    return true; // TODO: Implement semantic validation
}

std::vector<std::string> CHTLParser::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& error : errors_) {
        errors.push_back(ParserUtils::formatError(error));
    }
    return errors;
}

std::string CHTLParser::getErrorSummary() const {
    if (errors_.empty()) {
        return "No errors";
    }
    
    std::ostringstream oss;
    oss << "Found " << errors_.size() << " error(s):\n";
    
    for (size_t i = 0; i < errors_.size(); ++i) {
        oss << "  " << (i + 1) << ". " << ParserUtils::formatError(errors_[i]) << "\n";
    }
    
    return oss.str();
}

// ParserFactory implementation
std::unique_ptr<CHTLParser> ParserFactory::createParser() {
    return std::make_unique<CHTLParser>();
}

std::unique_ptr<CHTLParser> ParserFactory::createStrictParser() {
    auto parser = std::make_unique<CHTLParser>();
    parser->setStrictMode(true);
    return parser;
}

std::unique_ptr<CHTLParser> ParserFactory::createDebugParser() {
    auto parser = std::make_unique<CHTLParser>();
    parser->setDebugMode(true);
    return parser;
}

// ParserUtils implementation
bool ParserUtils::isElementStart(const Token& token) {
    return token.getType() == TokenType::IDENTIFIER;
}

bool ParserUtils::isTemplateStart(const Token& token) {
    return token.getType() == TokenType::AT_SYMBOL;
}

bool ParserUtils::isCustomStart(const Token& token) {
    return token.getType() == TokenType::BRACKET_OPEN;
}

bool ParserUtils::isImportStart(const Token& token) {
    return token.getType() == TokenType::BRACKET_OPEN;
}

bool ParserUtils::isNamespaceStart(const Token& token) {
    return token.getType() == TokenType::BRACKET_OPEN;
}

bool ParserUtils::isOriginStart(const Token& token) {
    return token.getType() == TokenType::BRACKET_OPEN;
}

std::string ParserUtils::formatError(const ParserError& error) {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(error.type) << "] ";
    
    if (error.line > 0) {
        oss << "Line " << error.line;
        if (error.column > 0) {
            oss << ":" << error.column;
        }
        oss << " - ";
    }
    
    oss << error.message;
    
    if (!error.context.empty()) {
        oss << " (Context: " << error.context << ")";
    }
    
    return oss.str();
}

std::string ParserUtils::formatErrorSummary(const std::vector<ParserError>& errors) {
    if (errors.empty()) {
        return "No errors found";
    }
    
    std::ostringstream oss;
    oss << "Found " << errors.size() << " error(s):\n";
    
    for (size_t i = 0; i < errors.size(); ++i) {
        oss << "  " << (i + 1) << ". " << formatError(errors[i]) << "\n";
    }
    
    return oss.str();
}

bool ParserUtils::isValidElementName(const std::string& name) {
    if (name.empty()) return false;
    
    // Must start with letter
    if (!std::isalpha(name[0])) return false;
    
    // Rest must be alphanumeric or dash
    for (size_t i = 1; i < name.length(); ++i) {
        if (!std::isalnum(name[i]) && name[i] != '-') {
            return false;
        }
    }
    
    return true;
}

bool ParserUtils::isValidTemplateName(const std::string& name) {
    return isValidElementName(name);
}

bool ParserUtils::isValidCustomName(const std::string& name) {
    return isValidElementName(name);
}

bool ParserUtils::isValidAttributeName(const std::string& name) {
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

bool ParserUtils::isValidAttributeValue(const std::string& value) {
    return !value.empty();
}

// State Handler implementations
void ParserInitHandler::onEnter(StateContext<ParserState>& context) {
    context.setData("status", "initialized");
}

void ParserInitHandler::onUpdate(StateContext<ParserState>& context) {
    // Initialize parser state
}

void ParserUseHandler::onEnter(StateContext<ParserState>& context) {
    context.setData("status", "parsing_use");
}

void ParserUseHandler::onUpdate(StateContext<ParserState>& context) {
    // Parse use statement
}

void ParserElementsHandler::onEnter(StateContext<ParserState>& context) {
    context.setData("status", "parsing_elements");
}

void ParserElementsHandler::onUpdate(StateContext<ParserState>& context) {
    // Parse elements
}

void ParserCompleteHandler::onEnter(StateContext<ParserState>& context) {
    context.setData("status", "complete");
}

void ParserCompleteHandler::onUpdate(StateContext<ParserState>& context) {
    // Handle completion
}

void ParserErrorHandler::onEnter(StateContext<ParserState>& context) {
    context.setData("status", "error");
}

void ParserErrorHandler::onUpdate(StateContext<ParserState>& context) {
    // Handle error state
}

} // namespace CHTL