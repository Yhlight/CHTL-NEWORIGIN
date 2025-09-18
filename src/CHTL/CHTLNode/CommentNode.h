#pragma once

#include "BaseNode.h"
#include "../CHTLNode/ASTVisitor.h"
#include <string>

namespace CHTL {

enum class CommentType {
    Line,       // // comment
    Block,      // /* comment */
    Generator   // # comment
};

class CommentNode : public BaseNode {
public:
    std::string content;
    CommentType commentType;

    CommentNode(const std::string& content, CommentType type)
        : content(content), commentType(type) {}

    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Comment; }
};

} // namespace CHTL
