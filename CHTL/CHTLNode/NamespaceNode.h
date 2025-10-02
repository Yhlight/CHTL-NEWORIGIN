#ifndef CHTL_NAMESPACE_NODE_H
#define CHTL_NAMESPACE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class NamespaceNode : public BaseNode {
public:
    explicit NamespaceNode(const std::string& name) : name(name) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        std::string indent(depth * 2, ' ');
        ss << indent << "NamespaceNode: " << name << " {\n";
        for (const auto& child : children) {
            ss << child->toString(depth + 1);
        }
        ss << indent << "}\n";
        return ss.str();
    }

    const std::string& getName() const { return name; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }

private:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif // CHTL_NAMESPACE_NODE_H