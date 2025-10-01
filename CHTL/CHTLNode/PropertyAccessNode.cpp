#include "PropertyAccessNode.h"
#include <iostream>

namespace CHTL {

PropertyAccessNode::PropertyAccessNode(const std::string& selector, const std::string& propertyName)
    : selector(selector), propertyName(propertyName) {}

ExpressionType PropertyAccessNode::getType() const {
    return ExpressionType::PROPERTY_ACCESS;
}

void PropertyAccessNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "PropertyAccess: " << selector << "." << propertyName << "\n";
}

std::string PropertyAccessNode::getSelector() const {
    return selector;
}

std::string PropertyAccessNode::getPropertyName() const {
    return propertyName;
}

} // namespace CHTL