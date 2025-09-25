#include "TextNode.h"

TextNode::TextNode(const std::string& text) : text(text) {}

NodeType TextNode::getType() const {
    return NodeType::Text;
}
