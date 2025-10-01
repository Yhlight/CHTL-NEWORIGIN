#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// Forward declarations
class CHTLContext;

// Base node class for AST
class CHTLNode {
public:
    enum class Type {
        ELEMENT,
        TEXT,
        COMMENT,
        TEMPLATE,
        CUSTOM,
        STYLE,
        SCRIPT,
        ORIGIN,
        IMPORT,
        CONFIG,
        NAMESPACE,
        ATTRIBUTE,
        EXPRESSION
    };

    CHTLNode(Type type, const std::string& name = "");
    virtual ~CHTLNode() = default;
    
    // Node properties
    Type getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // Children management
    void addChild(std::unique_ptr<CHTLNode> child);
    CHTLNode* getChild(size_t index) const;
    size_t getChildCount() const;
    std::vector<CHTLNode*> getChildren() const;
    
    // Attribute management
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    // Position information
    void setPosition(int line, int column);
    int getLine() const { return line_; }
    int getColumn() const { return column_; }
    
    // Validation
    virtual bool validate(CHTLContext& context) const;
    
    // Visitor pattern support
    virtual void accept(class CHTLNodeVisitor& visitor) const;

protected:
    Type type_;
    std::string name_;
    std::vector<std::unique_ptr<CHTLNode>> children_;
    std::unordered_map<std::string, std::string> attributes_;
    int line_;
    int column_;
};

// Concrete node implementations
class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& tagName);
    
    const std::string& getTagName() const { return getName(); }
    void setTagName(const std::string& tagName) { setName(tagName); }
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;
};

class TextNode : public CHTLNode {
public:
    TextNode(const std::string& content);
    
    const std::string& getContent() const { return getName(); }
    void setContent(const std::string& content) { setName(content); }
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;
};

class TemplateNode : public CHTLNode {
public:
    enum class TemplateType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    TemplateNode(TemplateType type, const std::string& name);
    
    TemplateType getTemplateType() const { return templateType_; }
    void setTemplateType(TemplateType type) { templateType_ = type; }
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;

private:
    TemplateType templateType_;
};

class CustomNode : public CHTLNode {
public:
    enum class CustomType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    CustomNode(CustomType type, const std::string& name);
    
    CustomType getCustomType() const { return customType_; }
    void setCustomType(CustomType type) { customType_ = type; }
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;

private:
    CustomType customType_;
};

class StyleNode : public CHTLNode {
public:
    StyleNode();
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;
};

class ScriptNode : public CHTLNode {
public:
    ScriptNode();
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;
};

class ImportNode : public CHTLNode {
public:
    enum class ImportType {
        HTML,
        STYLE,
        JAVASCRIPT,
        CHTL,
        CONFIG,
        CJMOD
    };
    
    ImportNode(ImportType type, const std::string& path);
    
    ImportType getImportType() const { return importType_; }
    void setImportType(ImportType type) { importType_ = type; }
    
    const std::string& getPath() const { return getName(); }
    void setPath(const std::string& path) { setName(path); }
    
    bool validate(CHTLContext& context) const override;
    void accept(class CHTLNodeVisitor& visitor) const override;

private:
    ImportType importType_;
};

// Visitor interface for AST traversal
class CHTLNodeVisitor {
public:
    virtual ~CHTLNodeVisitor() = default;
    
    virtual void visit(const ElementNode& node) = 0;
    virtual void visit(const TextNode& node) = 0;
    virtual void visit(const TemplateNode& node) = 0;
    virtual void visit(const CustomNode& node) = 0;
    virtual void visit(const StyleNode& node) = 0;
    virtual void visit(const ScriptNode& node) = 0;
    virtual void visit(const ImportNode& node) = 0;
    virtual void visit(const CHTLNode& node) = 0; // Fallback for base class
};

} // namespace CHTL