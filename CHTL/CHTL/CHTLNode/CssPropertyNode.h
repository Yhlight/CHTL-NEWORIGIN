#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CssPropertyNode : public BaseNode {
public:
    std::string property;
    std::string value;

    CssPropertyNode(const std::string& prop, const std::string& val)
        : property(prop), value(val) {}

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
