/**
 * @file BaseNode.h
 * @brief Base class for all AST nodes
 * 
 * Defines the interface for AST nodes in CHTL
 */

#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * @enum NodeType
 * @brief Types of AST nodes
 */
enum class NodeType {
    ELEMENT,        // HTML element
    TEXT,           // Text content
    COMMENT,        // Comment
    STYLE,          // Style block
    SCRIPT,         // Script block
    TEMPLATE,       // Template definition
    CUSTOM,         // Custom definition
    ORIGIN,         // Origin block
    IMPORT,         // Import statement
    NAMESPACE,      // Namespace
    CONFIG,         // Configuration
    ATTRIBUTE,      // Attribute
    PROPERTY        // CSS property
};

/**
 * @class BaseNode
 * @brief Base class for all AST nodes
 */
class BaseNode : public std::enable_shared_from_this<BaseNode> {
public:
    explicit BaseNode(NodeType type);
    virtual ~BaseNode() = default;
    
    // Node type
    NodeType getType() const { return type_; }
    std::string getTypeName() const;
    
    // Parent-child relationships
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    void setParent(std::shared_ptr<BaseNode> parent) { parent_ = parent; }
    std::shared_ptr<BaseNode> getParent() const { return parent_.lock(); }
    
    // Attributes (for elements)
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // Code generation
    virtual std::string toHTML() const;
    virtual std::string toCSS() const;
    virtual std::string toJS() const;
    
    // Visitor pattern support
    virtual void accept(class NodeVisitor& visitor);
    
    // Debug
    virtual std::string toString() const;
    std::string toTreeString(int depth = 0) const;

protected:
    NodeType type_;
    std::weak_ptr<BaseNode> parent_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::unordered_map<std::string, std::string> attributes_;
    
    // Helper for indentation
    std::string indent(int level) const;
};

/**
 * @class NodeVisitor
 * @brief Visitor pattern interface for traversing AST
 */
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visit(class ElementNode& node) = 0;
    virtual void visit(class TextNode& node) = 0;
    virtual void visit(class CommentNode& node) = 0;
    virtual void visit(class StyleNode& node) = 0;
    virtual void visit(class ScriptNode& node) = 0;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H
