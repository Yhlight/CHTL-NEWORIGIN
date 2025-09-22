#pragma once

#include "BaseNode.h"

// Represents a generator comment, e.g., `# a comment`
// These are preserved in the output, unlike // or /*...*/
class CommentNode : public BaseNode {
public:
    std::string content;

    explicit CommentNode(std::string content) : content(std::move(content)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }
};
