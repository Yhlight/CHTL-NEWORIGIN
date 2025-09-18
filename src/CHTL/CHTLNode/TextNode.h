#pragma once

#include "BaseNode.h"
#include "../CHTLNode/ASTVisitor.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& content) : content(content) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Text; }
};

} // namespace CHTL
