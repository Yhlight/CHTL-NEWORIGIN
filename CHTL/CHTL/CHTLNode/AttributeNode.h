#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    std::string key;
    std::string value;

    AttributeNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
