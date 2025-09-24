#include "ParserStrategy.h"
#include "../CHTLLexer/TokenStream.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../../Util/StringUtil/StringUtil.h"

namespace CHTL {

// UseStatementParser implementation
bool UseStatementParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::KEYWORD_USE;
}

std::unique_ptr<BaseNode> UseStatementParser::parse(TokenStream& stream) {
    if (!parseUseKeyword(stream)) {
        return nullptr;
    }
    
    if (!parseHtml5Keyword(stream)) {
        return nullptr;
    }
    
    if (!parseBlockStart(stream)) {
        return nullptr;
    }
    
    // Create a use statement node
    auto useNode = std::make_unique<BaseNode>(NodeType::BASE);
    useNode->setProperty("type", "use");
    useNode->setProperty("target", "html5");
    
    return useNode;
}

bool UseStatementParser::parseUseKeyword(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected 'use' keyword";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::KEYWORD_USE) {
        errorMessage_ = "Expected 'use' keyword, got: " + token.getValue();
        return false;
    }
    
    return true;
}

bool UseStatementParser::parseHtml5Keyword(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected 'html5' keyword after 'use'";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::KEYWORD_HTML5) {
        errorMessage_ = "Expected 'html5' keyword, got: " + token.getValue();
        return false;
    }
    
    return true;
}

bool UseStatementParser::parseBlockStart(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected '{' after 'use html5'";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::BRACE_OPEN) {
        errorMessage_ = "Expected '{', got: " + token.getValue();
        return false;
    }
    
    return true;
}

// ElementParser implementation
bool ElementParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::IDENTIFIER;
}

std::unique_ptr<BaseNode> ElementParser::parse(TokenStream& stream) {
    std::string elementName = parseElementName(stream);
    if (elementName.empty()) {
        return nullptr;
    }
    
    auto element = std::make_unique<ElementNode>(elementName);
    
    if (!parseElementBlock(stream, element.get())) {
        return nullptr;
    }
    
    return element;
}

std::string ElementParser::parseElementName(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected element name";
        return "";
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::IDENTIFIER) {
        errorMessage_ = "Expected element name, got: " + token.getValue();
        return "";
    }
    
    return token.getValue();
}

bool ElementParser::parseElementBlock(TokenStream& stream, ElementNode* element) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected '{' after element name";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::BRACE_OPEN) {
        errorMessage_ = "Expected '{', got: " + token.getValue();
        return false;
    }
    
    // Parse element content
    while (stream.hasMore()) {
        const Token& nextToken = stream.peek();
        
        if (nextToken.getType() == TokenType::BRACE_CLOSE) {
            stream.consume(); // Consume the closing brace
            break;
        }
        
        auto content = parseElementContent(stream);
        if (content) {
            element->addChild(std::move(content));
        }
    }
    
    return true;
}

std::unique_ptr<BaseNode> ElementParser::parseElementContent(TokenStream& stream) {
    if (!stream.hasMore()) {
        return nullptr;
    }
    
    const Token& token = stream.peek();
    
    // Handle different types of element content
    switch (token.getType()) {
        case TokenType::IDENTIFIER:
            // This could be an attribute or nested element
            // For now, treat as attribute
            return nullptr; // TODO: Implement attribute parsing
            
        case TokenType::KEYWORD_TEXT:
            // Text content
            return nullptr; // TODO: Implement text parsing
            
        case TokenType::KEYWORD_STYLE:
            // Style block
            return nullptr; // TODO: Implement style parsing
            
        case TokenType::KEYWORD_SCRIPT:
            // Script block
            return nullptr; // TODO: Implement script parsing
            
        default:
            errorMessage_ = "Unexpected token in element content: " + token.getValue();
            return nullptr;
    }
}

// AttributeParser implementation
bool AttributeParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    // Attributes are identifiers followed by colon
    return currentToken.getType() == TokenType::IDENTIFIER && 
           stream.hasMore() && 
           stream.peek().getType() == TokenType::OPERATOR_COLON;
}

std::unique_ptr<BaseNode> AttributeParser::parse(TokenStream& stream) {
    std::string attributeName = parseAttributeName(stream);
    if (attributeName.empty()) {
        return nullptr;
    }
    
    if (!parseAttributeAssignment(stream)) {
        return nullptr;
    }
    
    std::string attributeValue = parseAttributeValue(stream);
    if (attributeValue.empty()) {
        return nullptr;
    }
    
    // Create attribute node
    auto attributeNode = std::make_unique<BaseNode>(NodeType::ATTRIBUTE);
    attributeNode->setProperty("name", attributeName);
    attributeNode->setProperty("value", attributeValue);
    
    return attributeNode;
}

std::string AttributeParser::parseAttributeName(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected attribute name";
        return "";
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::IDENTIFIER) {
        errorMessage_ = "Expected attribute name, got: " + token.getValue();
        return "";
    }
    
    return token.getValue();
}

bool AttributeParser::parseAttributeAssignment(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected ':' after attribute name";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::OPERATOR_COLON) {
        errorMessage_ = "Expected ':', got: " + token.getValue();
        return false;
    }
    
    return true;
}

std::string AttributeParser::parseAttributeValue(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected attribute value";
        return "";
    }
    
    const Token& token = stream.consume();
    
    switch (token.getType()) {
        case TokenType::STRING_LITERAL:
            return token.getValue();
            
        case TokenType::UNQUOTED_LITERAL:
            return token.getValue();
            
        case TokenType::NUMERIC_LITERAL:
            return token.getValue();
            
        default:
            errorMessage_ = "Expected attribute value, got: " + token.getValue();
            return "";
    }
}

// TemplateParser implementation
bool TemplateParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::AT_SYMBOL &&
           stream.hasMore() &&
           stream.peek().getType() == TokenType::IDENTIFIER;
}

std::unique_ptr<BaseNode> TemplateParser::parse(TokenStream& stream) {
    // Parse @ symbol
    stream.consume();
    
    std::string templateType = parseTemplateType(stream);
    if (templateType.empty()) {
        return nullptr;
    }
    
    std::string templateName = parseTemplateName(stream);
    if (templateName.empty()) {
        return nullptr;
    }
    
    auto templateNode = std::make_unique<TemplateNode>(templateType, templateName);
    
    if (!parseTemplateBlock(stream, templateNode.get())) {
        return nullptr;
    }
    
    return templateNode;
}

std::string TemplateParser::parseTemplateType(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected template type after '@'";
        return "";
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::IDENTIFIER) {
        errorMessage_ = "Expected template type, got: " + token.getValue();
        return "";
    }
    
    return token.getValue();
}

std::string TemplateParser::parseTemplateName(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected template name";
        return "";
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::IDENTIFIER) {
        errorMessage_ = "Expected template name, got: " + token.getValue();
        return "";
    }
    
    return token.getValue();
}

bool TemplateParser::parseTemplateBlock(TokenStream& stream, TemplateNode* template) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected '{' after template name";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::BRACE_OPEN) {
        errorMessage_ = "Expected '{', got: " + token.getValue();
        return false;
    }
    
    // TODO: Parse template content
    // For now, just consume until closing brace
    while (stream.hasMore()) {
        const Token& nextToken = stream.peek();
        if (nextToken.getType() == TokenType::BRACE_CLOSE) {
            stream.consume();
            break;
        }
        stream.consume(); // Skip content for now
    }
    
    return true;
}

// CustomParser implementation
bool CustomParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::BRACKET_OPEN &&
           stream.hasMore() &&
           stream.peek().getType() == TokenType::KEYWORD_CUSTOM;
}

std::unique_ptr<BaseNode> CustomParser::parse(TokenStream& stream) {
    // Parse [ symbol
    stream.consume();
    
    // Parse Custom keyword
    stream.consume();
    
    std::string customType = parseCustomType(stream);
    if (customType.empty()) {
        return nullptr;
    }
    
    std::string customName = parseCustomName(stream);
    if (customName.empty()) {
        return nullptr;
    }
    
    auto customNode = std::make_unique<CustomNode>(customType, customName);
    
    if (!parseCustomBlock(stream, customNode.get())) {
        return nullptr;
    }
    
    return customNode;
}

std::string CustomParser::parseCustomType(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected custom type";
        return "";
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::IDENTIFIER) {
        errorMessage_ = "Expected custom type, got: " + token.getValue();
        return "";
    }
    
    return token.getValue();
}

std::string CustomParser::parseCustomName(TokenStream& stream) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected custom name";
        return "";
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::IDENTIFIER) {
        errorMessage_ = "Expected custom name, got: " + token.getValue();
        return "";
    }
    
    return token.getValue();
}

bool CustomParser::parseCustomBlock(TokenStream& stream, CustomNode* custom) {
    if (!stream.hasMore()) {
        errorMessage_ = "Expected '{' after custom name";
        return false;
    }
    
    const Token& token = stream.consume();
    if (token.getType() != TokenType::BRACE_OPEN) {
        errorMessage_ = "Expected '{', got: " + token.getValue();
        return false;
    }
    
    // TODO: Parse custom content
    // For now, just consume until closing brace
    while (stream.hasMore()) {
        const Token& nextToken = stream.peek();
        if (nextToken.getType() == TokenType::BRACE_CLOSE) {
            stream.consume();
            break;
        }
        stream.consume(); // Skip content for now
    }
    
    return true;
}

// ImportParser implementation
bool ImportParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::BRACKET_OPEN &&
           stream.hasMore() &&
           stream.peek().getType() == TokenType::KEYWORD_IMPORT;
}

std::unique_ptr<BaseNode> ImportParser::parse(TokenStream& stream) {
    // TODO: Implement import parsing
    return nullptr;
}

std::string ImportParser::parseImportType(TokenStream& stream) {
    // TODO: Implement
    return "";
}

std::string ImportParser::parseImportPath(TokenStream& stream) {
    // TODO: Implement
    return "";
}

std::string ImportParser::parseImportAlias(TokenStream& stream) {
    // TODO: Implement
    return "";
}

// NamespaceParser implementation
bool NamespaceParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::BRACKET_OPEN &&
           stream.hasMore() &&
           stream.peek().getType() == TokenType::KEYWORD_NAMESPACE;
}

std::unique_ptr<BaseNode> NamespaceParser::parse(TokenStream& stream) {
    // TODO: Implement namespace parsing
    return nullptr;
}

std::string NamespaceParser::parseNamespaceName(TokenStream& stream) {
    // TODO: Implement
    return "";
}

bool NamespaceParser::parseNamespaceBlock(TokenStream& stream) {
    // TODO: Implement
    return false;
}

// OriginParser implementation
bool OriginParser::canHandle(const Token& currentToken, const TokenStream& stream) const {
    return currentToken.getType() == TokenType::BRACKET_OPEN &&
           stream.hasMore() &&
           stream.peek().getType() == TokenType::KEYWORD_ORIGIN;
}

std::unique_ptr<BaseNode> OriginParser::parse(TokenStream& stream) {
    // TODO: Implement origin parsing
    return nullptr;
}

std::string OriginParser::parseOriginType(TokenStream& stream) {
    // TODO: Implement
    return "";
}

std::string OriginParser::parseOriginName(TokenStream& stream) {
    // TODO: Implement
    return "";
}

bool OriginParser::parseOriginContent(TokenStream& stream) {
    // TODO: Implement
    return false;
}

} // namespace CHTL