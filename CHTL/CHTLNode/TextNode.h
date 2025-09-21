#pragma once

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include "../CHTLGenerator/Visitor.h"
#include <string>

namespace CHTL {

// Represents a text node in the CHTL document, like `text { "Hello" }`.
class TextNode : public BaseNode {
public:
    Token content; // The token containing the text content (can be IDENTIFIER or STRING).

    void accept(Visitor& visitor) override;
};

} // namespace CHTL
