#include "OriginNode.h"
#include <iostream>

namespace CHTL {

OriginNode::OriginNode(const std::string& type, const std::string& content)
    : type(type), content(content) {}

void OriginNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "Origin @" << type << " { " << content << " }\n";
}

std::string OriginNode::getType() const {
    return type;
}

std::string OriginNode::getContent() const {
    return content;
}

} // namespace CHTL