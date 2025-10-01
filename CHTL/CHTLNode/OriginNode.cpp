#include "OriginNode.h"
#include <iostream>

namespace CHTL {

OriginNode::OriginNode(OriginType type, const std::string& content)
    : m_type(type), m_content(content) {}

void OriginNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::string type_str;
    switch (m_type) {
        case OriginType::HTML: type_str = "Html"; break;
        case OriginType::STYLE: type_str = "Style"; break;
        case OriginType::JAVASCRIPT: type_str = "JavaScript"; break;
    }
    std::cout << "Origin: @" << type_str << " {" << m_content << "}" << std::endl;
}

OriginType OriginNode::getOriginType() const {
    return m_type;
}

const std::string& OriginNode::getContent() const {
    return m_content;
}

} // namespace CHTL