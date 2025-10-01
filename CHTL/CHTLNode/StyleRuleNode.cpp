#include "StyleRuleNode.h"
#include <iostream>

namespace CHTL {

StyleRuleNode::StyleRuleNode(const std::string& selector)
    : selector(selector) {}

void StyleRuleNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "StyleRule: " << selector << " {\n";
    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "}\n";
}

std::string StyleRuleNode::getSelector() const {
    return selector;
}

const std::vector<std::unique_ptr<StylePropertyNode>>& StyleRuleNode::getProperties() const {
    return properties;
}

void StyleRuleNode::addProperty(std::unique_ptr<StylePropertyNode> property) {
    properties.push_back(std::move(property));
}

} // namespace CHTL