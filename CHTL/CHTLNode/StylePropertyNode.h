#ifndef CHTL_STYLE_PROPERTY_NODE_H
#define CHTL_STYLE_PROPERTY_NODE_H

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <memory>

namespace CHTL {

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& key, std::unique_ptr<ExpressionNode> value);
    void print(int indent = 0) const override;

    const std::string& getKey() const;
    const ExpressionNode* getValue() const;

private:
    std::string key;
    std::unique_ptr<ExpressionNode> value;
};

} // namespace CHTL

#endif // CHTL_STYLE_PROPERTY_NODE_H