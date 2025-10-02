/**
 * @file CommentNode.cpp
 * @brief Implementation of CommentNode
 */

#include "CommentNode.h"

namespace CHTL {

CommentNode::CommentNode(const std::string& content, bool isGenerator)
    : BaseNode(NodeType::COMMENT)
    , content_(content)
    , isGenerator_(isGenerator)
{
}

std::string CommentNode::toHTML() const {
    if (isGenerator_) {
        // Generator comments become HTML comments
        return "<!-- " + content_ + " -->";
    }
    // Regular CHTL comments are not included in output
    return "";
}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string CommentNode::toString() const {
    std::string type = isGenerator_ ? "Generator Comment" : "Comment";
    return type + ": " + content_;
}

} // namespace CHTL
