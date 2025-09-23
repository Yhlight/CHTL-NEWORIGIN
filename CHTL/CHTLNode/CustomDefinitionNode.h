#pragma once

#include "BaseNode.h"
#include "../CHTLExpr/BaseExprNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

enum class CustomType {
    Style,
    Element,
    Var
};

struct CustomDefinitionNode : public BaseNode {
    std::string name;
    CustomType customType;

    // For Style customs
    std::map<std::string, std::shared_ptr<BaseExprNode>> styleProperties;
    std::vector<std::string> valuelessProperties;

    // For Element customs
    std::vector<std::shared_ptr<BaseNode>> children;

    // For Var customs
    std::map<std::string, std::string> variables;

    CustomDefinitionNode(const std::string& name, CustomType type)
        : name(name), customType(type) {}

    NodeType getType() const override { return NodeType::CustomDefinition; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "CustomDefinitionNode (" << name << ")" << std::endl;
        for (const auto& child : children) {
            child->print(level + 1);
        }
    }
};
