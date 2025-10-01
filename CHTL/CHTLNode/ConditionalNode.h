#ifndef CHTL_CONDITIONAL_NODE_H
#define CHTL_CONDITIONAL_NODE_H

#include "BaseNode.h"
#include "IfNode.h"
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {

class ConditionalNode : public BaseNode {
public:
    std::vector<std::unique_ptr<IfNode>> ifClauses;
    std::vector<std::unique_ptr<BaseNode>> elseChildren;

    void print(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "ConditionalNode" << std::endl;
        for (const auto& clause : ifClauses) {
            clause->print(indent + 1);
        }
        if (!elseChildren.empty()) {
            for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
            std::cout << "ElseClause" << std::endl;
            for (const auto& child : elseChildren) {
                child->print(indent + 2);
            }
        }
    }

    // This method is not a virtual method in the base class, so `override` is removed.
    // Also, the logic here is likely incorrect as it adds children to the last `if` clause,
    // which might not be the desired behavior. For now, we will leave it as is,
    // but it might need to be revisited.
    void addChild(std::unique_ptr<BaseNode> node) {
        if (!ifClauses.empty()) {
            ifClauses.back()->addChild(std::move(node));
        }
    }
};

}

#endif //CHTL_CONDITIONAL_NODE_H