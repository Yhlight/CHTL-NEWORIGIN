#ifndef CHTL_IF_NODE_H
#define CHTL_IF_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <vector>
#include <memory>
#include <iostream>

namespace CHTL {

class IfNode : public BaseNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::unique_ptr<BaseNode>> children;

    void print(int indent = 0) const override {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "IfNode" << std::endl;
        if (condition) {
            for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
            std::cout << "Condition:" << std::endl;
            condition->print(indent + 2);
        }
        for (const auto& child : children) {
            child->print(indent + 1);
        }
    }

    void addChild(std::unique_ptr<BaseNode> node) {
        children.push_back(std::move(node));
    }
};

}

#endif //CHTL_IF_NODE_H