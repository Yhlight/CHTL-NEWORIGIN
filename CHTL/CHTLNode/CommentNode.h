#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Specifies the type of comment, which determines if it gets rendered in the output.
enum class CommentType {
    LINE,       // Standard single-line comment (e.g., //), ignored by generator.
    BLOCK,      // Standard multi-line comment (e.g., /* */), ignored by generator.
    GENERATOR   // A special comment (e.g., #) that gets rendered as an HTML comment.
};

// Represents a comment node in the CHTL structure.
class CommentNode : public BaseNode {
public:
    std::string content;
    CommentType type;

    void accept(NodeVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<CommentNode>(*this);
    }
};

} // namespace CHTL
