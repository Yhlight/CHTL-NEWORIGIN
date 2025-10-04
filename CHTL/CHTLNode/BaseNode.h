#pragma once

#include <vector>
#include <memory>

namespace CHTL {

    enum class NodeType {
        NODE_ELEMENT,
        NODE_TEXT,
        NODE_COMMENT,
        NODE_TEMPLATE,
        NODE_CUSTOM,
        NODE_STYLE,
        NODE_SCRIPT,
        NODE_ORIGIN,
        NODE_IMPORT,
        NODE_CONFIG,
        NODE_NAMESPACE,
        NODE_PROPERTY,
        NODE_RULE,
        NODE_OPERATOR,
        NODE_USE,
        NODE_TEMPLATE_USAGE
    };

    class BaseNode {
    public:
        virtual ~BaseNode() = default;
        NodeType getType() const { return type; }
        const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
        void addChild(std::shared_ptr<BaseNode> child) { children.push_back(child); }

    protected:
        NodeType type;
        std::vector<std::shared_ptr<BaseNode>> children;
    };

}