#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/CommentNode.h"

namespace CHTL
{
    std::string Generator::Generate(AST::BaseNode& root)
    {
        root.Accept(*this);
        return m_htmlStream.str();
    }

    void Generator::Indent()
    {
        for (int i = 0; i < m_indent; ++i)
        {
            m_htmlStream << "  ";
        }
    }

    void Generator::Visit(AST::ElementNode& node)
    {
        Indent();
        m_htmlStream << "<" << node.tagName;

        // Visit attributes
        for (const auto& attr : node.attributes)
        {
            attr->Accept(*this);
        }

        m_htmlStream << ">" << std::endl;

        // Visit children
        m_indent++;
        for (const auto& child : node.children)
        {
            child->Accept(*this);
        }
        m_indent--;

        Indent();
        m_htmlStream << "</" << node.tagName << ">" << std::endl;
    }

    void Generator::Visit(AST::AttributeNode& node)
    {
        // Note: This simple generator doesn't handle boolean attributes correctly.
        m_htmlStream << " " << node.name << "=\"" << node.value << "\"";
    }

    void Generator::Visit(AST::TextNode& node)
    {
        Indent();
        m_htmlStream << node.content << std::endl;
    }

    void Generator::Visit(AST::StyleNode& node)
    {
        // For this basic generator, we are not processing style nodes yet.
        // A more advanced version would collect this into a <style> tag or inline style attributes.
    }

    void Generator::Visit(AST::CommentNode& node)
    {
        // Only generate HTML comments for non-generator comments.
        // The CHTL spec implies '#' comments are for the generator and shouldn't be in the output.
        // We'd need to store comment type in the CommentNode to distinguish.
        // For now, we'll comment out all of them.
        Indent();
        m_htmlStream << "<!-- " << node.comment << " -->" << std::endl;
    }

} // namespace CHTL
