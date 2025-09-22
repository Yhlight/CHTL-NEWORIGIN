#pragma once

#include "BaseNode.h"
#include "Expression/ExpressionNode.h"
#include <vector>
#include <string>
#include <memory>
#include <utility>

namespace CHTL {

class IfNode : public BaseNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::vector<std::pair<std::string, std::unique_ptr<ExpressionNode>>> properties;

    NodeType getType() const override {
        return NodeType::If;
    }

    void addProperty(const std::string& key, std::unique_ptr<ExpressionNode> value) {
        properties.push_back({key, std::move(value)});
    }

    std::string toString(int indent = 0) const override {
        // Basic to string for debugging
        return std::string(indent * 2, ' ') + "IfNode (condition: " + condition->toString() + ")\n";
    }
};

} // namespace CHTL
