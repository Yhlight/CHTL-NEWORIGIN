#include "ElementNode.h"

ElementNode::ElementNode(const std::string& tagName) : tagName(tagName) {}

NodeType ElementNode::getType() const {
    return NodeType::Element;
}
