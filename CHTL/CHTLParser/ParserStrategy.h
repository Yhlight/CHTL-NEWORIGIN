#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../../Scanner/Strategy.h"
#include <memory>

namespace CHTL {

// Forward declarations
class TokenStream;
class ElementNode;
class TemplateNode;
class CustomNode;

// Base parser strategy interface
class ParserStrategy {
public:
    virtual ~ParserStrategy() = default;
    
    virtual bool canHandle(const Token& currentToken, const TokenStream& stream) const = 0;
    virtual std::unique_ptr<BaseNode> parse(TokenStream& stream) = 0;
    virtual std::string getName() const = 0;
    
    // Error handling
    virtual bool hasError() const { return false; }
    virtual std::string getError() const { return ""; }
    virtual void clearError() {}
    
protected:
    std::string errorMessage_;
};

// Use statement parser strategy
class UseStatementParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "UseStatementParser"; }
    
private:
    bool parseUseKeyword(TokenStream& stream);
    bool parseHtml5Keyword(TokenStream& stream);
    bool parseBlockStart(TokenStream& stream);
};

// Element parser strategy
class ElementParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "ElementParser"; }
    
private:
    std::string parseElementName(TokenStream& stream);
    bool parseElementBlock(TokenStream& stream, ElementNode* element);
    std::unique_ptr<BaseNode> parseElementContent(TokenStream& stream);
};

// Attribute parser strategy
class AttributeParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "AttributeParser"; }
    
private:
    std::string parseAttributeName(TokenStream& stream);
    std::string parseAttributeValue(TokenStream& stream);
    bool parseAttributeAssignment(TokenStream& stream);
};

// Template parser strategy
class TemplateParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "TemplateParser"; }
    
private:
    std::string parseTemplateType(TokenStream& stream);
    std::string parseTemplateName(TokenStream& stream);
    bool parseTemplateBlock(TokenStream& stream, TemplateNode* template);
};

// Custom parser strategy
class CustomParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "CustomParser"; }
    
private:
    std::string parseCustomType(TokenStream& stream);
    std::string parseCustomName(TokenStream& stream);
    bool parseCustomBlock(TokenStream& stream, CustomNode* custom);
};

// Import parser strategy
class ImportParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "ImportParser"; }
    
private:
    std::string parseImportType(TokenStream& stream);
    std::string parseImportPath(TokenStream& stream);
    std::string parseImportAlias(TokenStream& stream);
};

// Namespace parser strategy
class NamespaceParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "NamespaceParser"; }
    
private:
    std::string parseNamespaceName(TokenStream& stream);
    bool parseNamespaceBlock(TokenStream& stream);
};

// Origin parser strategy
class OriginParser : public ParserStrategy {
public:
    bool canHandle(const Token& currentToken, const TokenStream& stream) const override;
    std::unique_ptr<BaseNode> parse(TokenStream& stream) override;
    std::string getName() const override { return "OriginParser"; }
    
private:
    std::string parseOriginType(TokenStream& stream);
    std::string parseOriginName(TokenStream& stream);
    bool parseOriginContent(TokenStream& stream);
};

} // namespace CHTL