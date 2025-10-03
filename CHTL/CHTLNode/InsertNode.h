#ifndef CHTL_INSERT_NODE_H
#define CHTL_INSERT_NODE_H

#include "BaseNode.h"
#include "ElementNode.h"
#include <string>
#include <vector>
#include <memory>

enum class InsertPosition {
    After,
    Before,
    Replace,
    AtTop,
    AtBottom
};

class InsertNode : public BaseNode {
public:
    // Target can be null for AtTop/AtBottom
    InsertNode(InsertPosition pos, std::unique_ptr<ElementNode> target = nullptr)
        : position(pos), target(std::move(target)) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        childrenToInsert.push_back(std::move(child));
    }

    InsertPosition getPosition() const { return position; }
    const ElementNode* getTarget() const { return target.get(); }
    std::vector<std::unique_ptr<BaseNode>> takeChildren() { return std::move(childrenToInsert); }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedTarget = target ? std::unique_ptr<ElementNode>(static_cast<ElementNode*>(target->clone().release())) : nullptr;
        auto clonedNode = std::make_unique<InsertNode>(position, std::move(clonedTarget));
        for (const auto& child : childrenToInsert) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

    std::string toString(int depth = 0) const override { return "InsertNode"; }
    void accept(NodeVisitor& visitor) override { /* Not visited in final AST */ }
    std::string getNodeType() const override { return "InsertOperator"; }

private:
    InsertPosition position;
    std::unique_ptr<ElementNode> target;
    std::vector<std::unique_ptr<BaseNode>> childrenToInsert;
};

#endif // CHTL_INSERT_NODE_H