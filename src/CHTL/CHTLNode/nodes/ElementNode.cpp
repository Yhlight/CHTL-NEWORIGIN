#include "ElementNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const String& tagName, const SourceRange& range)
    : BaseNode(NodeType::Element, range), tagName_(tagName) {}

void ElementNode::setAttribute(const String& name, const String& value) {
    attributes_[name] = value;
}

Optional<String> ElementNode::getAttribute(const String& name) const {
    auto it = attributes_.find(name);
    if (it != attributes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool ElementNode::hasAttribute(const String& name) const {
    return attributes_.find(name) != attributes_.end();
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ElementNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Element(" << tagName_ << ")";

    if (!attributes_.empty()) {
        ss << " [";
        bool first = true;
        for (const auto& [name, value] : attributes_) {
            if (!first) ss << ", ";
            ss << name << "=\"" << value << "\"";
            first = false;
        }
        ss << "]";
    }

    if (!children_.empty()) {
        ss << "\n";
        for (const auto& child : children_) {
            ss << child->toString(indent + 1) << "\n";
        }
    }

    return ss.str();
}

SharedPtr<BaseNode> ElementNode::clone() const {
    auto node = std::make_shared<ElementNode>(tagName_, range_);
    node->attributes_ = attributes_;
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

} // namespace CHTL
