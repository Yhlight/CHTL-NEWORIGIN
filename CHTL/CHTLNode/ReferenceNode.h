#ifndef CHTL_REFERENCE_NODE_H
#define CHTL_REFERENCE_NODE_H

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class ReferenceNode : public ExpressionNode {
public:
    ReferenceNode(const std::string& selector, const std::string& property_name);

    ExpressionType getType() const override;
    void print(int indent = 0) const override;

    const std::string& getSelector() const;
    const std::string& getPropertyName() const;

private:
    std::string m_selector;
    std::string m_property_name;
};

} // namespace CHTL

#endif // CHTL_REFERENCE_NODE_H