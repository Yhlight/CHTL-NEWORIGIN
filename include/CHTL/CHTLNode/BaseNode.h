#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <any>

namespace CHTL {

/**
 * @brief Base class for all CHTL AST nodes
 * 
 * This is the foundation of the AST (Abstract Syntax Tree) for CHTL.
 * All syntax elements (elements, attributes, styles, etc.) inherit from this class.
 */
class BaseNode {
public:
    enum class NodeType {
        ELEMENT,
        TEXT,
        COMMENT,
        ATTRIBUTE,
        STYLE_BLOCK,
        SCRIPT_BLOCK,
        TEMPLATE_DEFINITION,
        CUSTOM_DEFINITION,
        IMPORT_STATEMENT,
        NAMESPACE_DEFINITION,
        CONFIGURATION,
        ORIGIN_BLOCK,
        OPERATOR_NODE,
        ROOT
    };

    BaseNode(NodeType type, const std::string& name = "");
    virtual ~BaseNode() = default;

    // Node information
    NodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    // Children management
    void addChild(std::unique_ptr<BaseNode> child);
    void removeChild(size_t index);
    void clearChildren();
    BaseNode* getChild(size_t index) const;
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children_; }
    size_t getChildCount() const { return children_.size(); }

    // Parent management
    BaseNode* getParent() const { return parent_; }
    void setParent(BaseNode* parent) { parent_ = parent; }

    // Properties management
    void setProperty(const std::string& key, const std::any& value);
    std::any getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;
    void removeProperty(const std::string& key);
    const std::unordered_map<std::string, std::any>& getProperties() const { return properties_; }

    // Position information
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }

    // Node validation
    virtual bool isValid() const;
    virtual std::string getValidationError() const;

    // Code generation
    virtual std::string generateHTML() const;
    virtual std::string generateCSS() const;
    virtual std::string generateJS() const;

    // Node traversal
    BaseNode* findChild(const std::string& name) const;
    std::vector<BaseNode*> findChildren(const std::string& name) const;
    BaseNode* findDescendant(const std::string& name) const;
    std::vector<BaseNode*> findDescendants(const std::string& name) const;

    // Utility methods
    virtual std::string toString() const;
    virtual std::string toDebugString() const;
    bool isLeaf() const { return children_.empty(); }
    bool isRoot() const { return type_ == NodeType::ROOT; }

    // Node cloning
    virtual std::unique_ptr<BaseNode> clone() const;

protected:
    NodeType type_;
    std::string name_;
    std::vector<std::unique_ptr<BaseNode>> children_;
    BaseNode* parent_;
    std::unordered_map<std::string, std::any> properties_;
    size_t line_;
    size_t column_;

    // Helper methods for derived classes
    virtual void validateChildren() const;
    virtual void validateProperties() const;
};

/**
 * @brief Root node for the entire CHTL document
 */
class RootNode : public BaseNode {
public:
    RootNode();
    ~RootNode() override = default;

    bool isValid() const override;
    std::string getValidationError() const override;
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    std::unique_ptr<BaseNode> clone() const override;

    // Document-level properties
    void setDocumentType(const std::string& doctype) { setProperty("doctype", doctype); }
    std::string getDocumentType() const;
    void setTitle(const std::string& title) { setProperty("title", title); }
    std::string getTitle() const;
    void setLanguage(const std::string& lang) { setProperty("language", lang); }
    std::string getLanguage() const;
};

/**
 * @brief Utility class for node operations
 */
class NodeUtils {
public:
    // Node searching
    static BaseNode* findNodeByType(BaseNode* root, BaseNode::NodeType type);
    static std::vector<BaseNode*> findAllNodesByType(BaseNode* root, BaseNode::NodeType type);
    static BaseNode* findNodeByProperty(BaseNode* root, const std::string& key, const std::any& value);
    static std::vector<BaseNode*> findAllNodesByProperty(BaseNode* root, const std::string& key, const std::any& value);

    // Node manipulation
    static void removeNode(BaseNode* node);
    static void replaceNode(BaseNode* oldNode, std::unique_ptr<BaseNode> newNode);
    static void insertNodeAfter(BaseNode* target, std::unique_ptr<BaseNode> newNode);
    static void insertNodeBefore(BaseNode* target, std::unique_ptr<BaseNode> newNode);

    // Node validation
    static bool validateNodeHierarchy(BaseNode* root);
    static std::vector<std::string> getValidationErrors(BaseNode* root);

    // Node serialization
    static std::string serializeToJSON(BaseNode* root);
    static std::unique_ptr<BaseNode> deserializeFromJSON(const std::string& json);

    // Node comparison
    static bool areNodesEqual(const BaseNode* node1, const BaseNode* node2);
    static bool areNodeTreesEqual(const BaseNode* root1, const BaseNode* root2);
};

} // namespace CHTL