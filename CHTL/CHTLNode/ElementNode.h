#pragma once

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLGenerator/Visitor.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents a single attribute of an element, e.g., id: "main".
struct Attribute {
    std::string key;
    Token value; // The token for the attribute's value (can be IDENTIFIER or STRING).
};

// Represents an element node in the CHTL document, like `div { ... }`.
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    void accept(Visitor& visitor) override;
};

} // namespace CHTL
