#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents the type of a comment in the source code.
enum class CommentType {
    SINGLE_LINE,      // // a comment
    MULTI_LINE,       // /* a comment */
    GENERATOR         // # a comment that should be rendered in the output
};

// An AST node representing a comment.
class CommentNode : public BaseNode {
public:
    std::string content;
    CommentType type;
};

} // namespace CHTL
