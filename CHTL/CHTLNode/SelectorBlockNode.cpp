#include "SelectorBlockNode.h"
#include <iostream>

namespace CHTL {

SelectorBlockNode::SelectorBlockNode(const std::string& selector) : m_selector(selector) {}

void SelectorBlockNode::addProperty(std::unique_ptr<StylePropertyNode> property) {
    m_properties.push_back(std::move(property));
}

const std::string& SelectorBlockNode::getSelector() const {
    return m_selector;
}

const std::vector<std::unique_ptr<StylePropertyNode>>& SelectorBlockNode::getProperties() const {
    return m_properties;
}

void SelectorBlockNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "SelectorBlock: " << m_selector << " {" << std::endl;
    for (const auto& prop : m_properties) {
        prop->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

} // namespace CHTL