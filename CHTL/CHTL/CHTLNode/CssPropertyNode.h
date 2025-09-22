#pragma once

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <memory>

namespace CHTL {

class CssPropertyNode : public BaseNode {
public:
    std::string property;
    std::unique_ptr<ExpressionNode> value;

    CssPropertyNode(const std::string& prop, std::unique_ptr<ExpressionNode> val)
        : property(prop), value(std::move(val)) {}

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
