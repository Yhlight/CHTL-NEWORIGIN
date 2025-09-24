#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/INodeVisitor.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL
{
    class Generator : public AST::INodeVisitor
    {
    public:
        Generator() = default;

        std::string Generate(AST::BaseNode& root);

        // Visitor methods
        void Visit(AST::ElementNode& node) override;
        void Visit(AST::AttributeNode& node) override;
        void Visit(AST::TextNode& node) override;
        void Visit(AST::StyleNode& node) override;
        void Visit(AST::CommentNode& node) override;

    private:
        std::stringstream m_htmlStream;
        std::stringstream m_cssStream;

        // Helper to manage indentation
        int m_indent = 0;
        void Indent();
    };
}

#endif // CHTL_GENERATOR_H
