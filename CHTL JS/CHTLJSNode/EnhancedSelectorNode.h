#pragma once

#include "CHTLJS_BaseNode.h"

namespace CHTL_JS {

// Represents an enhanced selector, e.g., {{.my-class}}
class EnhancedSelectorNode : public CHTLJS_BaseNode {
public:
    explicit EnhancedSelectorNode(std::string selector) : selector(std::move(selector)) {}

    void accept(CHTLJS_AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string selector;
};

} // namespace CHTL_JS
