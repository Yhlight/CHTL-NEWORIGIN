#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a text node in the CHTL structure.
class TextNode : public BaseNode {
public:
    std::string content;

    void accept(NodeVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

} // namespace CHTL
