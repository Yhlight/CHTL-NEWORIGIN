#include "CHTL/CHTLNode/ElementNode.h"
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName) 
    : BaseNode(NodeType::ELEMENT, tagName) {
}

std::string ElementNode::toHtml() const {
    std::stringstream ss;
    ss << "<" << getName();
    
    // Add attributes
    for (const auto& attr : getAttributes()) {
        ss << " " << attr.first << "=\"" << escapeHtml(attr.second) << "\"";
    }
    
    ss << ">";
    
    // Add content
    if (!getContent().empty()) {
        ss << escapeHtml(getContent());
    }
    
    // Add children
    for (const auto& child : getChildren()) {
        ss << child->toHtml();
    }
    
    ss << "</" << getName() << ">";
    return ss.str();
}

std::string ElementNode::toCss() const {
    return ""; // Elements don't generate CSS directly
}

std::string ElementNode::toJs() const {
    return ""; // Elements don't generate JS directly
}

bool ElementNode::isValid() const {
    return !getName().empty();
}

} // namespace CHTL