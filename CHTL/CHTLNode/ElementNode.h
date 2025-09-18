#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// A simple struct to hold key-value pairs for element attributes.
struct Attribute {
    std::string key;
    std::string value;
};

// Represents an element in the CHTL structure, like 'div' or 'span'.
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(NodeVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL
