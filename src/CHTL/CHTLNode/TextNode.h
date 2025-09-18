#pragma once

#include "BaseNode.h"
#include "AstVisitor.h"
#include <string>

namespace CHTL
{
    // Represents a block of text in the CHTL document.
    // e.g., text { "This is the content" }
    class TextNode : public BaseNode
    {
    public:
        std::string content;

        explicit TextNode(const std::string& text) : content(text) {}

        void accept(AstVisitor &visitor) override
        {
            visitor.visit(*this);
        }
    };
}
