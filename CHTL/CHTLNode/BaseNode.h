#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    enum class NodeType {
        NODE_ELEMENT,
        NODE_TEXT,
        NODE_STYLE,
        NODE_TEMPLATE,
        NODE_CUSTOM,
        NODE_ORIGIN,
        NODE_IMPORT,
        NODE_NAMESPACE,
        NODE_CONFIG,
        NODE_PROPERTY,
        NODE_RULE,
        NODE_USE,
        NODE_TEMPLATE_USAGE,
        NODE_IF,
        NODE_CONSTRAINT,
        NODE_DELETE,
        NODE_INSERT,
        NODE_SCRIPT,
        NODE_ANIMATE,

        // Expression Nodes
        NODE_EXPRESSION_LITERAL,
        NODE_EXPRESSION_UNARY,
        NODE_EXPRESSION_BINARY,
        NODE_EXPRESSION_GROUPING,
        NODE_EXPRESSION_VARIABLE
    };

    class BaseNode : public std::enable_shared_from_this<BaseNode> {
    private:
        NodeType type;
        std::weak_ptr<BaseNode> parent;
        std::vector<std::shared_ptr<BaseNode>> children;

    public:
        BaseNode(NodeType type) : type(type) {}
        virtual ~BaseNode() = default;
        NodeType getType() const { return type; }

        void setParent(const std::shared_ptr<BaseNode>& p) {
            this->parent = p;
        }

        std::shared_ptr<BaseNode> getParent() const {
            return parent.lock();
        }

        virtual void addChild(std::shared_ptr<BaseNode> child) {
            child->setParent(shared_from_this());
            children.push_back(child);
        }

        virtual const std::vector<std::shared_ptr<BaseNode>>& getChildren() const {
            return children;
        }

        virtual void setChildren(const std::vector<std::shared_ptr<BaseNode>>& new_children) {
            children = new_children;
            for(const auto& child : children) {
                child->setParent(shared_from_this());
            }
        }

        virtual std::string ToString(const std::string& indent = "", bool isLast = true) const {
            return indent + (isLast ? "`-- " : "|-- ") + "BaseNode\n";
        }
    };

}