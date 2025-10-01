#include "CssRuleNode.h"
#include <iostream>

namespace CHTL {

CssRuleNode::CssRuleNode(const std::string& selector) : selector(selector) {}

void CssRuleNode::addProperty(std::unique_ptr<StylePropertyNode> property) {
    properties.push_back(std::move(property));
}

const std::string& CssRuleNode::getSelector() const {
    return selector;
}

const std::vector<std::unique_ptr<StylePropertyNode>>& CssRuleNode::getProperties() const {
    return properties;
}

void CssRuleNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << selector << " {" << std::endl;

    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

} // namespace CHTL