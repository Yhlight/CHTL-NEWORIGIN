#ifndef CHTL_PROPERTYACCESSNODE_H
#define CHTL_PROPERTYACCESSNODE_H

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class PropertyAccessNode : public ExpressionNode {
public:
    PropertyAccessNode(const std::string& selector, const std::string& propertyName);
    ~PropertyAccessNode() override = default;
    PropertyAccessNode(PropertyAccessNode&&) noexcept = default;
    PropertyAccessNode& operator=(PropertyAccessNode&&) noexcept = default;

    ExpressionType getType() const override;
    void print(int indent = 0) const override;

    std::string getSelector() const;
    std::string getPropertyName() const;

private:
    std::string selector;
    std::string propertyName;
};

} // namespace CHTL

#endif // CHTL_PROPERTYACCESSNODE_H