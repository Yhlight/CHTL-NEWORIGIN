#pragma once

#include <string>

// Forward declarations of all node types
namespace CHTL {
    class ElementNode;
    class TextNode;
    class CommentNode;
    class AttributeNode;
    class StyleNode;
}

namespace CHTL {

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual std::string visit(ElementNode& node) = 0;
    virtual std::string visit(TextNode& node) = 0;
    virtual std::string visit(CommentNode& node) = 0;
    virtual std::string visit(AttributeNode& node) = 0;
    virtual std::string visit(StyleNode& node) = 0;
};

} // namespace CHTL
