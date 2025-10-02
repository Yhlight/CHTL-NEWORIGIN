/**
 * @file ElementNode.cpp
 * @brief Implementation of ElementNode
 */

#include "ElementNode.h"
#include <sstream>
#include <set>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName)
    : BaseNode(NodeType::ELEMENT)
    , tagName_(tagName)
    , selfClosing_(isSelfClosingTag(tagName))
{
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    
    // Opening tag
    oss << "<" << tagName_;
    
    // Attributes
    std::string attrs = attributesToString();
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    if (selfClosing_) {
        // Self-closing tag
        oss << " />";
    } else {
        oss << ">";
        
        // Children
        for (const auto& child : children_) {
            oss << child->toHTML();
        }
        
        // Closing tag
        oss << "</" << tagName_ << ">";
    }
    
    return oss.str();
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string ElementNode::toString() const {
    return "Element: " + tagName_;
}

std::string ElementNode::attributesToString() const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& [name, value] : attributes_) {
        if (!first) {
            oss << " ";
        }
        oss << name << "=\"" << value << "\"";
        first = false;
    }
    
    return oss.str();
}

bool ElementNode::isSelfClosingTag(const std::string& tag) const {
    static const std::set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return selfClosingTags.find(tag) != selfClosingTags.end();
}

} // namespace CHTL
