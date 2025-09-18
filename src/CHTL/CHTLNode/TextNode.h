#pragma once

#include "BaseNode.h"
#include "ASTVisitor.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& content) : content(content) {}

    std::string accept(ASTVisitor& visitor) override {
        return visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Text; }
};

} // namespace CHTL
