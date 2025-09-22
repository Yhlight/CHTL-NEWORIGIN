#pragma once

#include "BaseNode.h"
#include "CssPropertyNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleBlockNode : public BaseNode {
public:
    std::vector<std::unique_ptr<CssPropertyNode>> properties;

    StyleBlockNode() = default;

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
