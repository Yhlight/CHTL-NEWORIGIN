#include "CommentNode.h"
#include "AstVisitor.h"

namespace CHTL {

CommentNode::CommentNode(const std::string& text) : text(text) {}

void CommentNode::Accept(AstVisitor& visitor) {
    visitor.Visit(*this);
}

} // namespace CHTL
