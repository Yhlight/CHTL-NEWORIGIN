#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/CssPropertyNode.h"

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

        // Find and process any StyleNode children to create an inline style attribute
        std::stringstream styleAttrStream;
        for (const auto& child : node.children) {
            if (auto styleNode = dynamic_cast<AST::StyleNode*>(child.get())) {
                for(const auto& prop : styleNode->properties) {
                    styleAttrStream << prop->property << ": " << prop->value << ";";
                }
            }
        }
        if (styleAttrStream.str().length() > 0) {
            m_htmlStream << " style=\"" << styleAttrStream.str() << "\"";
        }

        m_htmlStream << ">" << std::endl;

        // Visit children, skipping any StyleNodes we just processed
        m_indent++;
        for (const auto& child : node.children)
        {
            if (dynamic_cast<AST::StyleNode*>(child.get()) == nullptr)
            {
                child->Accept(*this);
            }
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

    void Generator::Visit(AST::CssPropertyNode& node)
    {
        // This is handled by the StyleNode visitor.
        // This visit method is required to fulfill the visitor contract, but does nothing on its own.
    }

} // namespace CHTL
