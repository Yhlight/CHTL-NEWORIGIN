/**
 * @file StyleNode.cpp
 * @brief Implementation of StyleNode
 */

#include "StyleNode.h"
#include <sstream>

namespace CHTL {

StyleNode::StyleNode()
    : BaseNode(NodeType::STYLE)
    , inline_(true)
{
}

void StyleNode::addProperty(const std::string& name, const std::string& value) {
    properties_[name] = value;
}

std::string StyleNode::getProperty(const std::string& name) const {
    auto it = properties_.find(name);
    return (it != properties_.end()) ? it->second : "";
}

std::string StyleNode::toHTML() const {
    if (inline_) {
        // Inline style attribute
        std::ostringstream oss;
        bool first = true;
        for (const auto& [name, value] : properties_) {
            if (!first) {
                oss << "; ";
            }
            oss << name << ": " << value;
            first = false;
        }
        return oss.str();
    }
    return "";
}

std::string StyleNode::toCSS() const {
    if (inline_) {
        return ""; // Inline styles don't go to CSS file
    }
    
    std::ostringstream oss;
    oss << selector_ << " {\n";
    
    for (const auto& [name, value] : properties_) {
        oss << "  " << name << ": " << value << ";\n";
    }
    
    oss << "}\n";
    
    return oss.str();
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string StyleNode::toString() const {
    std::string type = inline_ ? "Inline Style" : "Style Block";
    if (!inline_ && !selector_.empty()) {
        type += " (" + selector_ + ")";
    }
    return type;
}

} // namespace CHTL
