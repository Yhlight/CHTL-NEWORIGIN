#pragma once

#include "BaseNode.h"
#include "AstVisitor.h"
#include <string>

namespace CHTL
{
    // Represents a comment in the CHTL document.
    class CommentNode : public BaseNode
    {
    public:
        enum class Type
        {
            REGULAR,   // For // and /*...*/ comments, which are ignored by the generator.
            GENERATOR, // For # comments, which are processed by the generator.
        };

        std::string content;
        Type type;

        CommentNode(const std::string& text, Type commentType) : content(text), type(commentType) {}

        void accept(AstVisitor &visitor) override
        {
            visitor.visit(*this);
        }
    };
}
