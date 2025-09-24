#include "CommentNode.h"
#include "../../Util/AstVisitor.h"

CommentNode::CommentNode(const std::string& content) : content(content) {}

void CommentNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

const std::string& CommentNode::getContent() const {
    return content;
}
