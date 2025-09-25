#include "CommentNode.h"

CommentNode::CommentNode(const std::string& text) : text(text) {}

NodeType CommentNode::getType() const {
    return NodeType::Comment;
}
