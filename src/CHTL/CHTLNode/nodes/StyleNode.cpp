#include "StyleNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

StyleNode::StyleNode(const SourceRange& range)
    : BaseNode(NodeType::Style, range) {}

void StyleNode::addInlineStyle(const String& property, const String& value) {
    inlineStyles_[property] = value;
}

void StyleNode::addCssRule(const String& selector, const String& rules) {
    cssRules_[selector] = rules;
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String StyleNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Style";

    if (!inlineStyles_.empty()) {
        ss << " Inline[";
        bool first = true;
        for (const auto& [prop, val] : inlineStyles_) {
            if (!first) ss << ", ";
            ss << prop << ": " << val;
            first = false;
        }
        ss << "]";
    }

    if (!cssRules_.empty()) {
        ss << " CSS[";
        bool first = true;
        for (const auto& [sel, rules] : cssRules_) {
            if (!first) ss << ", ";
            ss << sel << " {...}";
            first = false;
        }
        ss << "]";
    }

    return ss.str();
}

SharedPtr<BaseNode> StyleNode::clone() const {
    auto node = std::make_shared<StyleNode>(range_);
    node->inlineStyles_ = inlineStyles_;
    node->cssRules_ = cssRules_;
    return node;
}

} // namespace CHTL
