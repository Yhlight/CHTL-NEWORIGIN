#pragma once

#include "BaseNode.h"
#include "CssPropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CssRuleNode : public BaseNode {
public:
    std::string selector;
    std::vector<std::unique_ptr<CssPropertyNode>> properties;

    CssRuleNode(const std::string& sel) : selector(sel) {}

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
