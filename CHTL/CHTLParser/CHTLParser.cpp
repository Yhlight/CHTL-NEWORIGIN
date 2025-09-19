#include "CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser() : currentIndex_(0) {
}

CHTLParser::~CHTLParser() {
}

void CHTLParser::setTokens(const std::vector<Token>& tokens) {
    tokens_ = tokens;
    currentIndex_ = 0;
    errors_.clear();
    while (!nodeStack_.empty()) {
        nodeStack_.pop();
    }
}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    if (tokens_.empty()) {
        addError("No tokens to parse");
        return nullptr;
    }
    
    // 创建根节点
    auto root = std::make_shared<BaseNode>(NodeType::ELEMENT);
    root->setValue("root");
    
    nodeStack_.push(root);
    
    try {
        parseBlockContent(root);
    } catch (const std::exception& e) {
        addError("Parse error: " + std::string(e.what()));
    }
    
    return root;
}

const std::vector<std::string>& CHTLParser::getErrors() const {
    return errors_;
}

void CHTLParser::clear() {
    tokens_.clear();
    currentIndex_ = 0;
    errors_.clear();
    while (!nodeStack_.empty()) {
        nodeStack_.pop();
    }
}

Token CHTLParser::currentToken() const {
    if (currentIndex_ < tokens_.size()) {
        return tokens_[currentIndex_];
    }
    return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
}

Token CHTLParser::peekToken() const {
    if (currentIndex_ + 1 < tokens_.size()) {
        return tokens_[currentIndex_ + 1];
    }
    return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
}

bool CHTLParser::hasMoreTokens() const {
    return currentIndex_ < tokens_.size();
}

void CHTLParser::advance() {
    if (currentIndex_ < tokens_.size()) {
        currentIndex_++;
    }
}

bool CHTLParser::match(TokenType type) const {
    return currentToken().type == type;
}

bool CHTLParser::match(const std::string& value) const {
    return currentToken().value == value;
}

void CHTLParser::addError(const std::string& message) {
    errors_.push_back(message);
}

void CHTLParser::addError(const std::string& message, const Token& token) {
    std::stringstream ss;
    ss << message << " at token: " << token.value << " (type: " << static_cast<int>(token.type) << ")";
    errors_.push_back(ss.str());
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    if (!isElementStart()) {
        addError("Expected element start");
        return nullptr;
    }
    
    auto element = std::make_shared<BaseNode>(NodeType::ELEMENT);
    element->setValue(currentToken().value);
    advance();
    
    // 解析属性
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseAttributes(element);
    }
    
    // 解析子内容
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseBlockContent(element);
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after element content");
        }
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseTextNode() {
    if (!match(TokenType::STRING) && !match(TokenType::IDENTIFIER)) {
        addError("Expected text content");
        return nullptr;
    }
    
    auto textNode = std::make_shared<BaseNode>(NodeType::TEXT);
    textNode->setValue(currentToken().value);
    advance();
    
    return textNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    if (!match(TokenType::COMMENT)) {
        addError("Expected comment");
        return nullptr;
    }
    
    auto commentNode = std::make_shared<BaseNode>(NodeType::COMMENT);
    commentNode->setValue(currentToken().value);
    advance();
    
    return commentNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleBlock() {
    if (!match("style")) {
        addError("Expected 'style' keyword");
        return nullptr;
    }
    
    auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE);
    advance();
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseStyleProperties(styleNode);
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after style block");
        }
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseScriptBlock() {
    if (!match("script")) {
        addError("Expected 'script' keyword");
        return nullptr;
    }
    
    auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT);
    advance();
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        // 解析脚本内容
        std::string scriptContent;
        while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
            scriptContent += currentToken().value + " ";
            advance();
        }
        scriptNode->setValue(scriptContent);
        
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after script block");
        }
    }
    
    return scriptNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    if (!isTemplateStart()) {
        addError("Expected template start");
        return nullptr;
    }
    
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE);
    advance(); // 跳过 [Template]
    
    // 解析模板类型和名称
    if (match("@Style") || match("@Element") || match("@Var")) {
        templateNode->setValue(currentToken().value);
        advance();
        
        if (match(TokenType::IDENTIFIER)) {
            templateNode->setName(currentToken().value);
            advance();
        }
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseTemplateContent(templateNode);
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after template content");
        }
    }
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    if (!isCustomStart()) {
        addError("Expected custom start");
        return nullptr;
    }
    
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM);
    advance(); // 跳过 [Custom]
    
    // 解析自定义类型和名称
    if (match("@Style") || match("@Element") || match("@Var")) {
        customNode->setValue(currentToken().value);
        advance();
        
        if (match(TokenType::IDENTIFIER)) {
            customNode->setName(currentToken().value);
            advance();
        }
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseCustomContent(customNode);
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after custom content");
        }
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    if (!isOriginStart()) {
        addError("Expected origin start");
        return nullptr;
    }
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN);
    advance(); // 跳过 [Origin]
    
    // 解析原始类型和名称
    if (match("@Html") || match("@Style") || match("@JavaScript")) {
        originNode->setValue(currentToken().value);
        advance();
        
        if (match(TokenType::IDENTIFIER)) {
            originNode->setName(currentToken().value);
            advance();
        }
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        // 解析原始内容
        std::string originContent;
        while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
            originContent += currentToken().value + " ";
            advance();
        }
        originNode->setValue(originContent);
        
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after origin content");
        }
    }
    
    return originNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    if (!isImportStart()) {
        addError("Expected import start");
        return nullptr;
    }
    
    auto importNode = std::make_shared<BaseNode>(NodeType::IMPORT);
    advance(); // 跳过 [Import]
    
    // 解析导入类型和路径
    if (match("@Html") || match("@Style") || match("@JavaScript") || match("@Chtl") || match("@CJmod")) {
        importNode->setValue(currentToken().value);
        advance();
        
        if (match("from")) {
            advance();
            if (match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
                importNode->setName(currentToken().value);
                advance();
            }
        }
    }
    
    return importNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    if (!isNamespaceStart()) {
        addError("Expected namespace start");
        return nullptr;
    }
    
    auto namespaceNode = std::make_shared<BaseNode>(NodeType::NAMESPACE);
    advance(); // 跳过 [Namespace]
    
    if (match(TokenType::IDENTIFIER)) {
        namespaceNode->setName(currentToken().value);
        advance();
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseNamespaceContent(namespaceNode);
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after namespace content");
        }
    }
    
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    if (!isConfigurationStart()) {
        addError("Expected configuration start");
        return nullptr;
    }
    
    auto configNode = std::make_shared<BaseNode>(NodeType::CONFIG);
    advance(); // 跳过 [Configuration]
    
    if (match(TokenType::IDENTIFIER)) {
        configNode->setName(currentToken().value);
        advance();
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        advance();
        parseConfigurationContent(configNode);
        if (match(TokenType::RIGHT_BRACE)) {
            advance();
        } else {
            addError("Expected '}' after configuration content");
        }
    }
    
    return configNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseUse() {
    if (!isUseStart()) {
        addError("Expected use start");
        return nullptr;
    }
    
    auto useNode = std::make_shared<BaseNode>(NodeType::OPERATOR);
    advance(); // 跳过 use
    
    if (match(TokenType::IDENTIFIER) || match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
        useNode->setValue(currentToken().value);
        advance();
    }
    
    return useNode;
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> element) {
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
        if (match("style")) {
            auto styleNode = parseStyleBlock();
            if (styleNode) {
                element->addChild(styleNode);
            }
        } else if (match("script")) {
            auto scriptNode = parseScriptBlock();
            if (scriptNode) {
                element->addChild(scriptNode);
            } else {
                // 解析普通属性
                auto attr = parseAttribute();
                if (!attr.first.empty()) {
                    element->setAttribute(attr.first, attr.second);
                }
            }
        } else {
            // 解析普通属性
            auto attr = parseAttribute();
            if (!attr.first.empty()) {
                element->setAttribute(attr.first, attr.second);
            }
        }
        
        skipWhitespace();
    }
}

std::pair<std::string, std::string> CHTLParser::parseAttribute() {
    if (!match(TokenType::IDENTIFIER)) {
        return {"", ""};
    }
    
    std::string name = currentToken().value;
    advance();
    
    if (match(TokenType::COLON) || match(TokenType::ASSIGN)) {
        advance();
        
        std::string value;
        if (match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
            value = currentToken().value;
            advance();
        }
        
        return {name, value};
    }
    
    return {name, ""};
}

void CHTLParser::parseStyleProperties(std::shared_ptr<BaseNode> styleNode) {
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
        auto prop = parseStyleProperty();
        if (!prop.first.empty()) {
            styleNode->setAttribute(prop.first, prop.second);
        }
        
        skipWhitespace();
    }
}

std::pair<std::string, std::string> CHTLParser::parseStyleProperty() {
    if (!match(TokenType::IDENTIFIER)) {
        return {"", ""};
    }
    
    std::string name = currentToken().value;
    advance();
    
    if (match(TokenType::COLON)) {
        advance();
        
        std::string value;
        if (match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
            value = currentToken().value;
            advance();
        }
        
        return {name, value};
    }
    
    return {name, ""};
}

bool CHTLParser::isElementStart() const {
    return match(TokenType::IDENTIFIER) && !isTemplateStart() && !isCustomStart() && 
           !isOriginStart() && !isImportStart() && !isNamespaceStart() && 
           !isConfigurationStart() && !isUseStart();
}

bool CHTLParser::isTemplateStart() const {
    return match("[Template]");
}

bool CHTLParser::isCustomStart() const {
    return match("[Custom]");
}

bool CHTLParser::isOriginStart() const {
    return match("[Origin]");
}

bool CHTLParser::isImportStart() const {
    return match("[Import]");
}

bool CHTLParser::isNamespaceStart() const {
    return match("[Namespace]");
}

bool CHTLParser::isConfigurationStart() const {
    return match("[Configuration]");
}

bool CHTLParser::isUseStart() const {
    return match("use");
}

void CHTLParser::skipWhitespace() {
    while (hasMoreTokens() && (match(TokenType::WHITESPACE) || match(TokenType::NEWLINE))) {
        advance();
    }
}

void CHTLParser::skipComments() {
    while (hasMoreTokens() && match(TokenType::COMMENT)) {
        advance();
    }
}

std::string CHTLParser::parseStringLiteral() {
    if (match(TokenType::STRING)) {
        std::string value = currentToken().value;
        advance();
        return value;
    }
    return "";
}

std::string CHTLParser::parseUnquotedLiteral() {
    if (match(TokenType::IDENTIFIER)) {
        std::string value = currentToken().value;
        advance();
        return value;
    }
    return "";
}

std::string CHTLParser::parseSelector() {
    std::string selector;
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE) && !match(TokenType::COLON) && !match(TokenType::ASSIGN)) {
        selector += currentToken().value;
        advance();
    }
    return selector;
}

std::string CHTLParser::parseExpression() {
    std::string expression;
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE) && !match(TokenType::COLON) && !match(TokenType::ASSIGN)) {
        expression += currentToken().value;
        advance();
    }
    return expression;
}

void CHTLParser::parseBlockContent(std::shared_ptr<BaseNode> parent) {
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
        skipWhitespace();
        skipComments();
        
        if (!hasMoreTokens() || match(TokenType::RIGHT_BRACE)) {
            break;
        }
        
        std::shared_ptr<BaseNode> child = nullptr;
        
        if (isTemplateStart()) {
            child = parseTemplate();
        } else if (isCustomStart()) {
            child = parseCustom();
        } else if (isOriginStart()) {
            child = parseOrigin();
        } else if (isImportStart()) {
            child = parseImport();
        } else if (isNamespaceStart()) {
            child = parseNamespace();
        } else if (isConfigurationStart()) {
            child = parseConfiguration();
        } else if (isUseStart()) {
            child = parseUse();
        } else if (isElementStart()) {
            child = parseElement();
        } else if (match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
            child = parseTextNode();
        } else if (match(TokenType::COMMENT)) {
            child = parseComment();
        } else {
            addError("Unexpected token: " + currentToken().value);
            advance();
        }
        
        if (child) {
            parent->addChild(child);
        }
        
        skipWhitespace();
    }
}

void CHTLParser::parseTemplateContent(std::shared_ptr<BaseNode> templateNode) {
    parseBlockContent(templateNode);
}

void CHTLParser::parseCustomContent(std::shared_ptr<BaseNode> customNode) {
    parseBlockContent(customNode);
}

void CHTLParser::parseImportContent(std::shared_ptr<BaseNode> importNode) {
    parseBlockContent(importNode);
}

void CHTLParser::parseNamespaceContent(std::shared_ptr<BaseNode> namespaceNode) {
    parseBlockContent(namespaceNode);
}

void CHTLParser::parseConfigurationContent(std::shared_ptr<BaseNode> configNode) {
    parseBlockContent(configNode);
}

} // namespace CHTL