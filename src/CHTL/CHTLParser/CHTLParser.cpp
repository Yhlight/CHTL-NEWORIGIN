#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser() 
    : current(0), strictMode(false), debugMode(false) {
}

CHTLParser::~CHTLParser() {
}

std::shared_ptr<ASTNode> CHTLParser::parse(const std::string& source) {
    // 创建词法分析器
    CHTLLexer lexer(source, "");
    auto tokenVector = lexer.tokenize();
    tokens.clear();
    for (const auto& tokenPtr : tokenVector) {
        tokens.push_back(*tokenPtr);
    }
    
    if (tokens.empty()) {
        error("Empty input");
        return nullptr;
    }
    
    current = 0;
    errors.clear();
    
    debugPrint("Starting CHTL parsing");
    
    try {
        auto root = parseDocument();
        if (hasErrors()) {
            return nullptr;
        }
        
        debugPrint("CHTL parsing completed successfully");
        return root;
    } catch (const std::exception& e) {
        error("Parsing error: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<ASTNode> CHTLParser::parse(const std::vector<Token>& tokenList) {
    tokens = tokenList;
    current = 0;
    errors.clear();
    
    if (tokens.empty()) {
        error("Empty token list");
        return nullptr;
    }
    
    debugPrint("Starting CHTL parsing from tokens");
    
    try {
        auto root = parseDocument();
        if (hasErrors()) {
            return nullptr;
        }
        
        debugPrint("CHTL parsing from tokens completed successfully");
        return root;
    } catch (const std::exception& e) {
        error("Parsing error: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<ASTNode> CHTLParser::parseDocument() {
    debugPrint("Parsing document");
    
    auto root = std::make_shared<ASTNode>(NodeType::DOCUMENT, "document");
    nodeStack.push(root);
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        auto node = parseTopLevelElement();
        if (node) {
            root->addChild(node);
        } else {
            synchronize();
        }
    }
    
    nodeStack.pop();
    return root;
}

std::shared_ptr<ASTNode> CHTLParser::parseTopLevelElement() {
    skipWhitespace();
    if (isAtEnd()) return nullptr;
    
    Token token = peek();
    
    switch (token.getType()) {
        case TokenType::IDENTIFIER:
            return parseElement();
        case TokenType::COMMENT:
            return parseComment();
        case TokenType::GENERATOR_COMMENT:
            return parseGeneratorComment();
        case TokenType::TEMPLATE:
            return parseTemplate();
        case TokenType::CUSTOM:
            return parseCustom();
        case TokenType::ORIGIN:
            return parseOrigin();
        case TokenType::IMPORT:
            return parseImport();
        case TokenType::NAMESPACE:
            return parseNamespace();
        case TokenType::CONFIGURATION:
            return parseConfiguration();
        default:
            errorAt(token, "Unexpected token at top level: " + token.getValue());
            return nullptr;
    }
}

std::shared_ptr<ASTNode> CHTLParser::parseElement() {
    debugPrint("Parsing element");
    
    if (!match(TokenType::IDENTIFIER)) {
        errorAt(peek(), "Expected element identifier");
        return nullptr;
    }
    
    Token elementToken = advance();
    std::string elementName = elementToken.getValue();
    
    if (!isValidElementName(elementName)) {
        errorAt(elementToken, "Invalid element name: " + elementName);
        return nullptr;
    }
    
    auto element = std::make_shared<ASTNode>(NodeType::ELEMENT, elementName);
    
    // 解析左大括号
    if (!match(TokenType::LEFT_BRACE)) {
        errorAt(peek(), "Expected '{' after element name");
        return nullptr;
    }
    advance(); // 消耗 {
    
    // 解析属性和内容
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
        skipWhitespace();
        if (isAtEnd() || match(TokenType::RIGHT_BRACE)) break;
        
        Token token = peek();
        
        if (token.getType() == TokenType::IDENTIFIER && 
            (peek(1).getType() == TokenType::COLON || peek(1).getType() == TokenType::EQUAL)) {
            // 解析属性
            auto attributes = parseAttributes();
            for (const auto& attr : attributes) {
                auto attrNode = std::make_shared<ASTNode>(NodeType::ATTRIBUTE, attr.first);
                attrNode->setValue(attr.second);
                element->addChild(attrNode);
            }
        } else if (token.getValue() == "text") {
            // 解析文本节点
            auto textNode = parseTextNode();
            if (textNode) {
                element->addChild(textNode);
            }
        } else if (token.getValue() == "style") {
            // 解析局部样式块
            auto styleNode = parseLocalStyleBlock();
            if (styleNode) {
                element->addChild(styleNode);
            }
        } else if (token.getValue() == "script") {
            // 解析脚本块
            auto scriptNode = parseScriptBlock();
            if (scriptNode) {
                element->addChild(scriptNode);
            }
        } else if (token.getType() == TokenType::IDENTIFIER) {
            // 嵌套元素
            auto childElement = parseElement();
            if (childElement) {
                element->addChild(childElement);
            }
        } else {
            errorAt(token, "Unexpected token in element: " + token.getValue());
            synchronize();
        }
    }
    
    // 解析右大括号
    if (!match(TokenType::RIGHT_BRACE)) {
        errorAt(peek(), "Expected '}' to close element");
        return nullptr;
    }
    advance(); // 消耗 }
    
    debugPrint("Element parsed: " + elementName);
    return element;
}

std::shared_ptr<ASTNode> CHTLParser::parseTextNode() {
    debugPrint("Parsing text node");
    
    if (!match(TokenType::IDENTIFIER, "text")) {
        errorAt(peek(), "Expected 'text' keyword");
        return nullptr;
    }
    advance(); // 消耗 text
    
    // 解析左大括号
    if (!match(TokenType::LEFT_BRACE)) {
        errorAt(peek(), "Expected '{' after 'text'");
        return nullptr;
    }
    advance(); // 消耗 {
    
    // 解析文本内容
    std::string textContent;
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
        Token token = advance();
        if (token.getType() == TokenType::STRING_LITERAL || 
            token.getType() == TokenType::UNQUOTED_STRING) {
            textContent += token.getValue();
        } else if (token.getType() != TokenType::WHITESPACE) {
            textContent += token.getValue();
        }
    }
    
    // 解析右大括号
    if (!match(TokenType::RIGHT_BRACE)) {
        errorAt(peek(), "Expected '}' to close text node");
        return nullptr;
    }
    advance(); // 消耗 }
    
    auto textNode = std::make_shared<ASTNode>(NodeType::TEXT, "text");
    textNode->setValue(textContent);
    
    debugPrint("Text node parsed: " + textContent.substr(0, 50) + "...");
    return textNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseComment() {
    debugPrint("Parsing comment");
    
    Token token = advance(); // 消耗注释token
    auto commentNode = std::make_shared<ASTNode>(NodeType::COMMENT, "comment");
    commentNode->setValue(token.getValue());
    
    return commentNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseGeneratorComment() {
    debugPrint("Parsing generator comment");
    
    Token token = advance(); // 消耗生成器注释token
    auto commentNode = std::make_shared<ASTNode>(NodeType::GENERATOR_COMMENT, "generator_comment");
    commentNode->setValue(token.getValue());
    
    return commentNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseLocalStyleBlock() {
    debugPrint("Parsing local style block");
    
    if (!match(TokenType::IDENTIFIER, "style")) {
        errorAt(peek(), "Expected 'style' keyword");
        return nullptr;
    }
    advance(); // 消耗 style
    
    // 解析左大括号
    if (!match(TokenType::LEFT_BRACE)) {
        errorAt(peek(), "Expected '{' after 'style'");
        return nullptr;
    }
    advance(); // 消耗 {
    
    auto styleNode = std::make_shared<ASTNode>(NodeType::LOCAL_STYLE, "style");
    
    // 解析样式属性
    auto properties = parseStyleProperties();
    for (const auto& prop : properties) {
        auto propNode = std::make_shared<ASTNode>(NodeType::STYLE_PROPERTY, prop.first);
        propNode->setValue(prop.second);
        styleNode->addChild(propNode);
    }
    
    // 解析右大括号
    if (!match(TokenType::RIGHT_BRACE)) {
        errorAt(peek(), "Expected '}' to close style block");
        return nullptr;
    }
    advance(); // 消耗 }
    
    debugPrint("Local style block parsed");
    return styleNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseScriptBlock() {
    debugPrint("Parsing script block");
    
    if (!match(TokenType::IDENTIFIER, "script")) {
        errorAt(peek(), "Expected 'script' keyword");
        return nullptr;
    }
    advance(); // 消耗 script
    
    // 解析左大括号
    if (!match(TokenType::LEFT_BRACE)) {
        errorAt(peek(), "Expected '{' after 'script'");
        return nullptr;
    }
    advance(); // 消耗 {
    
    auto scriptNode = std::make_shared<ASTNode>(NodeType::SCRIPT, "script");
    
    // 解析脚本内容
    std::string scriptContent;
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
        Token token = advance();
        scriptContent += token.getValue() + " ";
    }
    
    scriptNode->setValue(scriptContent);
    
    // 解析右大括号
    if (!match(TokenType::RIGHT_BRACE)) {
        errorAt(peek(), "Expected '}' to close script block");
        return nullptr;
    }
    advance(); // 消耗 }
    
    debugPrint("Script block parsed");
    return scriptNode;
}

// 占位符实现 - 这些将在后续实现
std::shared_ptr<ASTNode> CHTLParser::parseTemplate() {
    debugPrint("Parsing template (placeholder)");
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseCustom() {
    debugPrint("Parsing custom (placeholder)");
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseOrigin() {
    debugPrint("Parsing origin (placeholder)");
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseImport() {
    debugPrint("Parsing import (placeholder)");
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseNamespace() {
    debugPrint("Parsing namespace (placeholder)");
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseConfiguration() {
    debugPrint("Parsing configuration (placeholder)");
    return nullptr;
}

std::map<std::string, std::string> CHTLParser::parseAttributes() {
    std::map<std::string, std::string> attributes;
    
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE) && !match(TokenType::LEFT_BRACE)) {
        skipWhitespace();
        if (isAtEnd() || match(TokenType::RIGHT_BRACE) || match(TokenType::LEFT_BRACE)) break;
        
        auto attr = parseAttribute();
        if (!attr.first.empty()) {
            attributes[attr.first] = attr.second;
        }
    }
    
    return attributes;
}

std::pair<std::string, std::string> CHTLParser::parseAttribute() {
    if (!match(TokenType::IDENTIFIER)) {
        return {"", ""};
    }
    
    Token nameToken = advance();
    std::string name = nameToken.getValue();
    
    if (!isValidAttributeName(name)) {
        errorAt(nameToken, "Invalid attribute name: " + name);
        return {"", ""};
    }
    
    // 解析冒号或等号
    if (!match(TokenType::COLON) && !match(TokenType::EQUAL)) {
        errorAt(peek(), "Expected ':' or '=' after attribute name");
        return {"", ""};
    }
    advance(); // 消耗 : 或 =
    
    // 解析属性值
    skipWhitespace();
    Token valueToken = advance();
    std::string value;
    
    if (valueToken.getType() == TokenType::STRING_LITERAL) {
        value = valueToken.getValue();
    } else if (valueToken.getType() == TokenType::UNQUOTED_STRING) {
        value = valueToken.getValue();
    } else {
        errorAt(valueToken, "Expected attribute value");
        return {"", ""};
    }
    
    // 解析分号
    skipWhitespace();
    if (match(TokenType::SEMICOLON)) {
        advance(); // 消耗 ;
    }
    
    return {name, value};
}

std::map<std::string, std::string> CHTLParser::parseStyleProperties() {
    std::map<std::string, std::string> properties;
    
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
        skipWhitespace();
        if (isAtEnd() || match(TokenType::RIGHT_BRACE)) break;
        
        auto prop = parseStyleProperty();
        if (!prop.first.empty()) {
            properties[prop.first] = prop.second;
        }
    }
    
    return properties;
}

std::pair<std::string, std::string> CHTLParser::parseStyleProperty() {
    if (!match(TokenType::IDENTIFIER)) {
        return {"", ""};
    }
    
    Token nameToken = advance();
    std::string name = nameToken.getValue();
    
    if (!isValidCSSProperty(name)) {
        errorAt(nameToken, "Invalid CSS property: " + name);
        return {"", ""};
    }
    
    // 解析冒号
    if (!match(TokenType::COLON)) {
        errorAt(peek(), "Expected ':' after CSS property name");
        return {"", ""};
    }
    advance(); // 消耗 :
    
    // 解析属性值
    skipWhitespace();
    std::string value;
    
    while (!isAtEnd() && !match(TokenType::SEMICOLON) && !match(TokenType::RIGHT_BRACE)) {
        Token token = advance();
        if (token.getType() != TokenType::WHITESPACE || !value.empty()) {
            value += token.getValue();
        }
    }
    
    // 解析分号
    if (match(TokenType::SEMICOLON)) {
        advance(); // 消耗 ;
    }
    
    return {name, value};
}

// 辅助方法实现
bool CHTLParser::match(TokenType type) {
    if (isAtEnd()) return false;
    return peek().getType() == type;
}

bool CHTLParser::match(const std::string& value) {
    if (isAtEnd()) return false;
    return peek().getValue() == value;
}

bool CHTLParser::match(TokenType type, const std::string& value) {
    if (isAtEnd()) return false;
    return peek().getType() == type && peek().getValue() == value;
}

Token CHTLParser::consume(TokenType type) {
    if (match(type)) {
        return advance();
    }
    errorAt(peek(), "Expected token type: " + std::to_string(static_cast<int>(type)));
    return Token{TokenType::ERROR, "", 0, 0};
}

Token CHTLParser::consume(TokenType type, const std::string& value) {
    if (match(type, value)) {
        return advance();
    }
    errorAt(peek(), "Expected: " + value);
    return Token{TokenType::ERROR, "", 0, 0};
}

Token CHTLParser::peek() const {
    if (isAtEnd()) {
        return Token{TokenType::EOF_TOKEN, "", 0, 0};
    }
    return tokens[current];
}

Token CHTLParser::peek(size_t offset) const {
    if (current + offset >= tokens.size()) {
        return Token{TokenType::EOF_TOKEN, "", 0, 0};
    }
    return tokens[current + offset];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size();
}

void CHTLParser::skipWhitespace() {
    while (!isAtEnd() && peek().getType() == TokenType::WHITESPACE) {
        advance();
    }
}

void CHTLParser::error(const std::string& message) {
    errors.push_back(message);
    if (errorHandler) {
        errorHandler(message, current);
    }
    debugPrint("ERROR: " + message);
}

void CHTLParser::errorAt(const Token& token, const std::string& message) {
    std::string fullMessage = "At line " + std::to_string(token.getLine()) + 
                             ", column " + std::to_string(token.getColumn()) + 
                             ": " + message;
    errors.push_back(fullMessage);
    if (errorHandler) {
        errorHandler(fullMessage, current);
    }
    debugPrint("ERROR: " + fullMessage);
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (peek().getType() == TokenType::RIGHT_BRACE) {
            return;
        }
        
        switch (peek().getType()) {
            case TokenType::IDENTIFIER:
                return;
            default:
                advance();
                break;
        }
    }
}

bool CHTLParser::isValidElementName(const std::string& name) {
    // 基本的HTML元素名称验证
    if (name.empty()) return false;
    
    // 检查是否是有效的HTML元素名称
    static const std::vector<std::string> validElements = {
        "html", "head", "body", "title", "div", "span", "p", "h1", "h2", "h3", 
        "h4", "h5", "h6", "a", "img", "button", "input", "form", "table", 
        "tr", "td", "th", "ul", "ol", "li", "nav", "header", "footer", 
        "main", "section", "article", "aside", "script", "style", "link"
    };
    
    return std::find(validElements.begin(), validElements.end(), name) != validElements.end() ||
           !strictMode; // 非严格模式下允许自定义元素
}

bool CHTLParser::isValidAttributeName(const std::string& name) {
    if (name.empty()) return false;
    
    // 基本属性名称验证
    static const std::vector<std::string> validAttributes = {
        "id", "class", "src", "href", "alt", "title", "type", "value", 
        "placeholder", "required", "disabled", "checked", "selected",
        "style", "onclick", "onload", "onchange", "onsubmit"
    };
    
    return std::find(validAttributes.begin(), validAttributes.end(), name) != validAttributes.end() ||
           !strictMode; // 非严格模式下允许自定义属性
}

bool CHTLParser::isValidCSSProperty(const std::string& property) {
    if (property.empty()) return false;
    
    // 基本CSS属性验证
    static const std::vector<std::string> validProperties = {
        "color", "background-color", "width", "height", "margin", "padding",
        "border", "display", "position", "top", "left", "right", "bottom",
        "font-size", "font-family", "text-align", "line-height", "opacity",
        "z-index", "float", "clear", "overflow", "visibility", "cursor"
    };
    
    return std::find(validProperties.begin(), validProperties.end(), property) != validProperties.end() ||
           !strictMode; // 非严格模式下允许自定义CSS属性
}

void CHTLParser::setErrorHandler(std::function<void(const std::string&, size_t)> handler) {
    errorHandler = handler;
}

void CHTLParser::debugPrint(const std::string& message) {
    if (debugMode) {
        std::cout << "[CHTLParser] " << message << std::endl;
    }
}

void CHTLParser::debugPrintToken(const Token& token) {
    if (debugMode) {
        std::cout << "[CHTLParser] Token: " << static_cast<int>(token.getType()) 
                  << " '" << token.getValue() << "'" << std::endl;
    }
}

// ParserUtils 实现
namespace ParserUtils {

bool validateCHTLSyntax(const std::string& source) {
    CHTLParser parser;
    auto result = parser.parse(source);
    return result != nullptr && !parser.hasErrors();
}

DocumentInfo extractDocumentInfo(const std::string& source) {
    DocumentInfo info;
    info.hasRootElement = false;
    
    CHTLParser parser;
    auto result = parser.parse(source);
    
    if (!result || parser.hasErrors()) {
        return info;
    }
    
    // 提取文档信息的逻辑将在后续实现
    return info;
}

bool checkElementNesting(const std::string& source) {
    // 检查元素嵌套的逻辑将在后续实现
    return true;
}

bool checkAttributeSyntax(const std::string& source) {
    // 检查属性语法的逻辑将在后续实现
    return true;
}

bool checkStyleSyntax(const std::string& source) {
    // 检查样式语法的逻辑将在后续实现
    return true;
}

bool checkScriptSyntax(const std::string& source) {
    // 检查脚本语法的逻辑将在后续实现
    return true;
}

} // namespace ParserUtils

} // namespace CHTL