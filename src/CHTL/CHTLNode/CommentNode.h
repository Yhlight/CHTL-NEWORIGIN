#pragma once

#include "BaseNode.h"
#include <string>

// Represents a comment node in the AST.
// In CHTL, only '#' comments are turned into nodes, as they are
// rendered into the final output. Other comments are ignored by the lexer.
class CommentNode : public BaseNode {
public:
    explicit CommentNode(const std::string& text);

    NodeType getType() const override;

    // The content of the comment.
    std::string text;
};
