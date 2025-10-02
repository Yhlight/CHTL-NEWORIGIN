/**
 * @file BaseNode.cpp
 * @brief Implementation of BaseNode
 */

#include "BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type)
    : type_(type)
{
}

std::string BaseNode::getTypeName() const {
    switch (type_) {
        case NodeType::ELEMENT: return "Element";
        case NodeType::TEXT: return "Text";
        case NodeType::COMMENT: return "Comment";
        case NodeType::STYLE: return "Style";
        case NodeType::SCRIPT: return "Script";
        case NodeType::TEMPLATE: return "Template";
        case NodeType::CUSTOM: return "Custom";
        case NodeType::ORIGIN: return "Origin";
        case NodeType::IMPORT: return "Import";
        case NodeType::NAMESPACE: return "Namespace";
        case NodeType::CONFIG: return "Config";
        case NodeType::ATTRIBUTE: return "Attribute";
        case NodeType::PROPERTY: return "Property";
        default: return "Unknown";
    }
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string BaseNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

std::string BaseNode::toHTML() const {
    std::string result;
    for (const auto& child : children_) {
        result += child->toHTML();
    }
    return result;
}

std::string BaseNode::toCSS() const {
    std::string result;
    for (const auto& child : children_) {
        result += child->toCSS();
    }
    return result;
}

std::string BaseNode::toJS() const {
    std::string result;
    for (const auto& child : children_) {
        result += child->toJS();
    }
    return result;
}

void BaseNode::accept(NodeVisitor& /* visitor */) {
    // Default implementation - to be overridden by derived classes
}

std::string BaseNode::toString() const {
    return getTypeName() + " Node";
}

std::string BaseNode::toTreeString(int depth) const {
    std::ostringstream oss;
    oss << indent(depth) << toString() << "\n";
    
    for (const auto& child : children_) {
        oss << child->toTreeString(depth + 1);
    }
    
    return oss.str();
}

std::string BaseNode::indent(int level) const {
    return std::string(level * 2, ' ');
}

} // namespace CHTL
