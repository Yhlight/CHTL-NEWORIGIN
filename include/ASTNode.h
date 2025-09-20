#pragma once

#include "CHTLCommon.h"
#include "Token.h"
#include <variant>

namespace CHTL {

class ASTNode {
private:
    NodeType type;
    String value;
    StringVector attributes;
    ASTNodeVector children;
    std::shared_ptr<Token> token;
    StringMap metadata;
    int depth;

public:
    ASTNode(NodeType t, const String& v = "", std::shared_ptr<Token> tok = nullptr)
        : type(t), value(v), token(tok), depth(0) {}
    
    // Getters
    NodeType getType() const { return type; }
    const String& getValue() const { return value; }
    const StringVector& getAttributes() const { return attributes; }
    const ASTNodeVector& getChildren() const { return children; }
    std::shared_ptr<Token> getToken() const { return token; }
    const StringMap& getMetadata() const { return metadata; }
    int getDepth() const { return depth; }
    
    // Setters
    void setType(NodeType t) { type = t; }
    void setValue(const String& v) { value = v; }
    void setAttributes(const StringVector& attrs) { attributes = attrs; }
    void setToken(std::shared_ptr<Token> tok) { token = tok; }
    void setMetadata(const StringMap& meta) { metadata = meta; }
    void setDepth(int d) { depth = d; }
    
    // Child management
    void addChild(std::shared_ptr<ASTNode> child);
    void removeChild(std::shared_ptr<ASTNode> child);
    void insertChild(int index, std::shared_ptr<ASTNode> child);
    std::shared_ptr<ASTNode> getChild(int index) const;
    size_t getChildCount() const;
    bool hasChildren() const;
    
    // Attribute management
    void addAttribute(const String& attr);
    void removeAttribute(const String& attr);
    bool hasAttribute(const String& attr) const;
    
    // Metadata management
    void setMetadata(const String& key, const String& value);
    String getMetadata(const String& key) const;
    bool hasMetadata(const String& key) const;
    
    // Utility methods
    bool isLeaf() const;
    bool isContainer() const;
    String toString() const;
    String getTypeString() const;
    
    // Tree traversal
    std::shared_ptr<ASTNode> findFirst(NodeType type) const;
    ASTNodeVector findAll(NodeType type) const;
    std::shared_ptr<ASTNode> findFirstWithValue(const String& value) const;
    ASTNodeVector findAllWithValue(const String& value) const;
    
    // Validation
    bool isValid() const;
    StringVector validate() const;
};

// Specialized node types
class ElementNode : public ASTNode {
private:
    String tagName;
    StringMap attributes;
    String textContent;
    
public:
    ElementNode(const String& tag, std::shared_ptr<Token> tok = nullptr)
        : ASTNode(NodeType::ELEMENT, tag, tok), tagName(tag) {}
    
    const String& getTagName() const { return tagName; }
    void setTagName(const String& name) { tagName = name; setValue(name); }
    
    const StringMap& getAttributes() const { return attributes; }
    void setAttribute(const String& key, const String& val) { attributes[key] = val; }
    String getAttribute(const String& key) const;
    bool hasAttribute(const String& key) const;
    void removeAttribute(const String& key);
    
    const String& getTextContent() const { return textContent; }
    void setTextContent(const String& text) { textContent = text; }
};

class TextNode : public ASTNode {
private:
    String content;
    bool isLiteral;
    
public:
    TextNode(const String& text, bool literal = true, std::shared_ptr<Token> tok = nullptr)
        : ASTNode(NodeType::TEXT, text, tok), content(text), isLiteral(literal) {}
    
    const String& getContent() const { return content; }
    void setContent(const String& text) { content = text; setValue(text); }
    
    bool isLiteralContent() const { return isLiteral; }
    void setLiteral(bool literal) { isLiteral = literal; }
};

class StyleNode : public ASTNode {
private:
    StringMap properties;
    StringVector selectors;
    bool isGlobal;
    
public:
    StyleNode(bool global = false, std::shared_ptr<Token> tok = nullptr)
        : ASTNode(NodeType::STYLE_BLOCK, "", tok), isGlobal(global) {}
    
    const StringMap& getProperties() const { return properties; }
    void setProperty(const String& key, const String& value);
    String getProperty(const String& key) const;
    bool hasProperty(const String& key) const;
    void removeProperty(const String& key);
    
    const StringVector& getSelectors() const { return selectors; }
    void addSelector(const String& selector);
    void removeSelector(const String& selector);
    bool hasSelector(const String& selector) const;
    
    bool isGlobalStyle() const { return isGlobal; }
    void setGlobal(bool global) { isGlobal = global; }
};

class TemplateNode : public ASTNode {
private:
    String templateName;
    String templateType;
    StringMap parameters;
    bool isCustom;
    
public:
    TemplateNode(const String& name, const String& type, bool custom = false, std::shared_ptr<Token> tok = nullptr)
        : ASTNode(custom ? NodeType::CUSTOM_STYLE : NodeType::TEMPLATE_STYLE, name, tok),
          templateName(name), templateType(type), isCustom(custom) {}
    
    const String& getTemplateName() const { return templateName; }
    void setTemplateName(const String& name) { templateName = name; setValue(name); }
    
    const String& getTemplateType() const { return templateType; }
    void setTemplateType(const String& type) { templateType = type; }
    
    const StringMap& getParameters() const { return parameters; }
    void setParameter(const String& key, const String& value);
    String getParameter(const String& key) const;
    bool hasParameter(const String& key) const;
    
    bool isCustomTemplate() const { return isCustom; }
    void setCustom(bool custom) { isCustom = custom; }
};

} // namespace CHTL