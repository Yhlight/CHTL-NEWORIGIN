#pragma once

#include "CHTLJS_BaseNode.h"

namespace CHTL_JS {

// Represents a block of raw, unprocessed JavaScript code.
class RawJSNode : public CHTLJS_BaseNode {
public:
    explicit RawJSNode(std::string js_code) : js_code(std::move(js_code)) {}

    void accept(CHTLJS_AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string js_code;
};

} // namespace CHTL_JS
