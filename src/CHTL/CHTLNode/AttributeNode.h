#pragma once

#include "BaseNode.h"
#include "ASTVisitor.h"
#include <string>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    std::string key;
    std::string value;

    AttributeNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    std::string accept(ASTVisitor& visitor) override {
        return visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Attribute; }
};

} // namespace CHTL
