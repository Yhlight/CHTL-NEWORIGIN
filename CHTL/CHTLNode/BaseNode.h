#pragma once

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL {

// Forward declarations
class BaseNode;
class ElementNode;
class TextNode;
class CommentNode;
class TemplateNode;
class CustomNode;
class StyleNode;
class ScriptNode;
class OriginNode;
class ImportNode;
class ConfigNode;
class NamespaceNode;
class OperatorNode;
class BehaviorNode;
class ConditionalNode;

// Node types
enum class NodeType {
    BASE,
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
    BEHAVIOR,
    CONDITIONAL,
    ATTRIBUTE,
    EXPRESSION,
    LITERAL,
    SELECTOR,
    FUNCTION_CALL,
    TEMPLATE_CALL,
    CUSTOM_CALL,
    VAR_CALL,
    ORIGIN_CALL,
    IMPORT_CALL,
    NAMESPACE_CALL,
    CONFIG_CALL
};

// Node visitor interface
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    // Base node visits
    virtual void visit(BaseNode* node) {}
    virtual void visit(ElementNode* node) {}
    virtual void visit(TextNode* node) {}
    virtual void visit(CommentNode* node) {}
    virtual void visit(TemplateNode* node) {}
    virtual void visit(CustomNode* node) {}
    virtual void visit(StyleNode* node) {}
    virtual void visit(ScriptNode* node) {}
    virtual void visit(OriginNode* node) {}
    virtual void visit(ImportNode* node) {}
    virtual void visit(ConfigNode* node) {}
    virtual void visit(NamespaceNode* node) {}
    virtual void visit(OperatorNode* node) {}
    virtual void visit(BehaviorNode* node) {}
    virtual void visit(ConditionalNode* node) {}
};

// Node position information
struct NodePosition {
    size_t line;
    size_t column;
    size_t length;
    
    NodePosition(size_t l = 0, size_t c = 0, size_t len = 0) 
        : line(l), column(c), length(len) {}
};

// Base node class
class BaseNode {
public:
    BaseNode(NodeType type = NodeType::BASE);
    virtual ~BaseNode() = default;
    
    // Type information
    NodeType getType() const { return type_; }
    virtual std::string getTypeName() const;
    
    // Position information
    NodePosition getPosition() const { return position_; }
    void setPosition(const NodePosition& pos) { position_ = pos; }
    void setPosition(size_t line, size_t column, size_t length = 0);
    
    // Parent-child relationships
    BaseNode* getParent() const { return parent_; }
    void setParent(BaseNode* parent) { parent_ = parent; }
    
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children_; }
    void addChild(std::unique_ptr<BaseNode> child);
    void removeChild(BaseNode* child);
    void clearChildren();
    
    // Sibling navigation
    BaseNode* getNextSibling() const;
    BaseNode* getPreviousSibling() const;
    size_t getChildIndex() const;
    
    // Tree traversal
    std::vector<BaseNode*> getDescendants() const;
    std::vector<BaseNode*> getAncestors() const;
    std::vector<BaseNode*> getSiblings() const;
    
    // Node properties
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;
    void removeProperty(const std::string& key);
    void clearProperties();
    
    const std::map<std::string, std::string>& getProperties() const { return properties_; }
    
    // Node metadata
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    void clearMetadata();
    
    const std::map<std::string, std::string>& getMetadata() const { return metadata_; }
    
    // Visitor pattern
    virtual void accept(NodeVisitor* visitor);
    
    // Node validation
    virtual bool isValid() const;
    virtual std::vector<std::string> getValidationErrors() const;
    
    // Node transformation
    virtual std::unique_ptr<BaseNode> clone() const;
    virtual std::string toString() const;
    virtual std::string toDebugString() const;
    
    // Node comparison
    virtual bool equals(const BaseNode* other) const;
    
    // Node search
    std::vector<BaseNode*> findNodes(NodeType type) const;
    std::vector<BaseNode*> findNodes(const std::function<bool(const BaseNode*)>& predicate) const;
    BaseNode* findFirstNode(NodeType type) const;
    BaseNode* findFirstNode(const std::function<bool(const BaseNode*)>& predicate) const;
    
    // Node filtering
    std::vector<BaseNode*> filterNodes(NodeType type) const;
    std::vector<BaseNode*> filterNodes(const std::function<bool(const BaseNode*)>& predicate) const;
    
    // Node statistics
    size_t getNodeCount() const;
    size_t getNodeCount(NodeType type) const;
    size_t getDepth() const;
    size_t getMaxDepth() const;
    
    // Node manipulation
    void replaceChild(BaseNode* oldChild, std::unique_ptr<BaseNode> newChild);
    void insertChild(size_t index, std::unique_ptr<BaseNode> child);
    std::unique_ptr<BaseNode> removeChildAt(size_t index);
    
    // Node serialization
    virtual std::string serialize() const;
    virtual void deserialize(const std::string& data);
    
protected:
    NodeType type_;
    NodePosition position_;
    BaseNode* parent_;
    std::vector<std::unique_ptr<BaseNode>> children_;
    std::map<std::string, std::string> properties_;
    std::map<std::string, std::string> metadata_;
    
    // Helper methods for derived classes
    virtual void onChildAdded(BaseNode* child);
    virtual void onChildRemoved(BaseNode* child);
    virtual void onPropertyChanged(const std::string& key, const std::string& value);
    virtual void onMetadataChanged(const std::string& key, const std::string& value);
};

// Node builder for fluent construction
class NodeBuilder {
public:
    NodeBuilder(NodeType type);
    
    NodeBuilder& position(size_t line, size_t column, size_t length = 0);
    NodeBuilder& property(const std::string& key, const std::string& value);
    NodeBuilder& metadata(const std::string& key, const std::string& value);
    NodeBuilder& child(std::unique_ptr<BaseNode> child);
    
    std::unique_ptr<BaseNode> build();
    
private:
    std::unique_ptr<BaseNode> node_;
};

// Node factory for creating nodes
class NodeFactory {
public:
    static std::unique_ptr<ElementNode> createElement(const std::string& name);
    static std::unique_ptr<TextNode> createText(const std::string& content);
    static std::unique_ptr<CommentNode> createComment(const std::string& content);
    static std::unique_ptr<TemplateNode> createTemplate(const std::string& type, const std::string& name);
    static std::unique_ptr<CustomNode> createCustom(const std::string& type, const std::string& name);
    static std::unique_ptr<StyleNode> createStyle();
    static std::unique_ptr<ScriptNode> createScript();
    static std::unique_ptr<OriginNode> createOrigin(const std::string& type, const std::string& name = "");
    static std::unique_ptr<ImportNode> createImport(const std::string& type, const std::string& path);
    static std::unique_ptr<ConfigNode> createConfig(const std::string& name = "");
    static std::unique_ptr<NamespaceNode> createNamespace(const std::string& name);
    static std::unique_ptr<OperatorNode> createOperator(const std::string& op);
    static std::unique_ptr<BehaviorNode> createBehavior(const std::string& type);
    static std::unique_ptr<ConditionalNode> createConditional(const std::string& condition);
};

// Node utilities
class NodeUtils {
public:
    // Tree operations
    static std::unique_ptr<BaseNode> copyTree(const BaseNode* root);
    static void mergeTrees(BaseNode* target, const BaseNode* source);
    static std::unique_ptr<BaseNode> extractSubtree(BaseNode* root, BaseNode* node);
    
    // Node analysis
    static std::vector<BaseNode*> getAllNodes(const BaseNode* root);
    static std::vector<BaseNode*> getNodesByType(const BaseNode* root, NodeType type);
    static std::map<NodeType, size_t> getNodeTypeCounts(const BaseNode* root);
    
    // Node validation
    static bool validateTree(const BaseNode* root);
    static std::vector<std::string> getTreeValidationErrors(const BaseNode* root);
    
    // Node transformation
    static void transformTree(BaseNode* root, std::function<std::unique_ptr<BaseNode>(BaseNode*)> transformer);
    static void pruneTree(BaseNode* root, std::function<bool(const BaseNode*)> predicate);
    
    // Node serialization
    static std::string serializeTree(const BaseNode* root);
    static std::unique_ptr<BaseNode> deserializeTree(const std::string& data);
    
    // Node comparison
    static bool treesEqual(const BaseNode* tree1, const BaseNode* tree2);
    static std::vector<BaseNode*> findDifferences(const BaseNode* tree1, const BaseNode* tree2);
};

} // namespace CHTL