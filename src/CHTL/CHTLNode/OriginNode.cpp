#include "OriginNode.h"

OriginNode::OriginNode(const std::string& type, const std::string& content, const std::string& name)
    : originType(type), content(content), name(name) {}

NodeType OriginNode::getType() const {
    return NodeType::Origin;
}

std::string OriginNode::getOriginType() const {
    return originType;
}

std::string OriginNode::getContent() const {
    return content;
}

std::string OriginNode::getName() const {
    return name;
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    return std::make_unique<OriginNode>(originType, content, name);
}