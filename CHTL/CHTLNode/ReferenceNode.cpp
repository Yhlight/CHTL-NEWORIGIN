#include "ReferenceNode.h"
#include <iostream>

namespace CHTL {

ReferenceNode::ReferenceNode(const std::string& selector, const std::string& property_name)
    : m_selector(selector), m_property_name(property_name) {}

ExpressionType ReferenceNode::getType() const {
    return ExpressionType::REFERENCE;
}

void ReferenceNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Reference: " << m_selector << "." << m_property_name << std::endl;
}

const std::string& ReferenceNode::getSelector() const {
    return m_selector;
}

const std::string& ReferenceNode::getPropertyName() const {
    return m_property_name;
}

} // namespace CHTL