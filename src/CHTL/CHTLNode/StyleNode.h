#pragma once

#include "BaseNode.h"
#include "ASTVisitor.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    // Using a map to store style properties, e.g., "width" -> "100px"
    std::map<std::string, std::string> properties;

    StyleNode() = default;

    void addProperty(const std::string& key, const std::string& value) {
        properties[key] = value;
    }

    std::string accept(ASTVisitor& visitor) override {
        return visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Style; }
};

} // namespace CHTL
