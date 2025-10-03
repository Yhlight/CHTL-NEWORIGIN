#ifndef CHTL_FRAGMENT_NODE_H
#define CHTL_FRAGMENT_NODE_H

#include "BaseNode.h"
#include <vector>
#include <memory>

// A temporary node to hold multiple children during parsing, e.g., when expanding a template.
// This node should not appear in the final AST.
class FragmentNode : public BaseNode {
public:
    FragmentNode() = default;

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

    // Moves children out of the fragment
    std::vector<std::unique_ptr<BaseNode>> takeChildren() {
        return std::move(children);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<FragmentNode>();
        for (const auto& child : children) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

    std::string toString(int depth = 0) const override {
        // Should not be called
        return "FragmentNode";
    }

    void accept(NodeVisitor& visitor) override {
        // Should not be called
    }

    std::string getNodeType() const override { return "Fragment"; }


private:
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif //CHTL_FRAGMENT_NODE_H