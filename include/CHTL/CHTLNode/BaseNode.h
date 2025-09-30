#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <sstream>

namespace CHTL {

/**
 * @brief Base class for all CHTL AST nodes
 * 
 * Implements the composite pattern for building the AST
 * Each node represents a different CHTL language construct
 */
class BaseNode : public std::enable_shared_from_this<BaseNode> {
public:
    enum class NodeType {
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
        OPERATOR,
        ATTRIBUTE,
        ROOT
    };
    
    BaseNode(NodeType type, const std::string& name = "");
    virtual ~BaseNode() = default;
    
    // Node properties
    NodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // Children management
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void removeChild(size_t index);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children_; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    size_t getChildCount() const { return children_.size(); }
    
    // Parent management
    std::shared_ptr<BaseNode> getParent() const { return parent_.lock(); }
    void setParent(std::shared_ptr<BaseNode> parent) { parent_ = parent; }
    
    // Attributes
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // Content
    void setContent(const std::string& content) { content_ = content; }
    const std::string& getContent() const { return content_; }
    
    // Position information
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    // Virtual methods for different node types
    virtual std::string toString() const;
    virtual std::string toHtml() const = 0;
    virtual std::string toCss() const = 0;
    virtual std::string toJs() const = 0;
    virtual bool isValid() const = 0;
    virtual void accept(std::function<void(const BaseNode&)> visitor) const;
    
    // Node traversal
    std::vector<std::shared_ptr<BaseNode>> findNodes(NodeType type) const;
    std::vector<std::shared_ptr<BaseNode>> findNodes(const std::string& name) const;
    std::shared_ptr<BaseNode> findFirstNode(NodeType type) const;
    std::shared_ptr<BaseNode> findFirstNode(const std::string& name) const;
    
    // Node manipulation
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    void replaceChild(size_t index, std::shared_ptr<BaseNode> newChild);
    void clearChildren();
    
    // Utility methods
    bool isLeaf() const { return children_.empty(); }
    bool isRoot() const { return parent_.expired(); }
    size_t getDepth() const;
    std::string getPath() const;

protected:
    NodeType type_;
    std::string name_;
    std::string content_;
    std::unordered_map<std::string, std::string> attributes_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::weak_ptr<BaseNode> parent_;
    size_t line_;
    size_t column_;
    
    // Helper methods
    std::string indent(size_t level) const;
    std::string escapeHtml(const std::string& text) const;
    std::string escapeCss(const std::string& text) const;
    std::string escapeJs(const std::string& text) const;
};

/**
 * @brief Root node for the AST
 */
class RootNode : public BaseNode {
public:
    RootNode();
    virtual ~RootNode() = default;
    
    std::string toHtml() const override;
    std::string toCss() const override;
    std::string toJs() const override;
    bool isValid() const override;
};

} // namespace CHTL