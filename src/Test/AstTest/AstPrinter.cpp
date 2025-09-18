#include "AstPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"

namespace CHTL
{
    void AstPrinter::print(const std::vector<std::unique_ptr<BaseNode>> &nodes)
    {
        std::cout << "--- AST ---" << std::endl;
        for (const auto &node : nodes)
        {
            node->accept(*this);
        }
        std::cout << "--- End of AST ---" << std::endl;
    }

    void AstPrinter::visit(ElementNode &node)
    {
        std::cout << indentation << "Element: <" << node.tagName << ">" << std::endl;

        if (!node.attributes.empty())
        {
            indent();
            std::cout << indentation << "Attributes:" << std::endl;
            indent();
            for (const auto &attr : node.attributes)
            {
                std::cout << indentation << attr.first << " = \"" << attr.second << "\"" << std::endl;
            }
            unindent();
            unindent();
        }

        if (!node.children.empty())
        {
            indent();
            std::cout << indentation << "Children:" << std::endl;
            indent();
            for (const auto &child : node.children)
            {
                child->accept(*this);
            }
            unindent();
            unindent();
        }
    }

    void AstPrinter::visit(TextNode &node)
    {
        std::cout << indentation << "Text: \"" << node.content << "\"" << std::endl;
    }

    void AstPrinter::visit(CommentNode &node)
    {
        std::string type = (node.type == CommentNode::Type::GENERATOR) ? "Generator Comment" : "Regular Comment";
        std::cout << indentation << type << ": \"" << node.content << "\"" << std::endl;
    }

    void AstPrinter::visit(StyleNode &node)
    {
        std::cout << indentation << "Style Block:" << std::endl;
        indent();
        for (const auto &prop : node.properties)
        {
            std::cout << indentation << prop.first << ": " << prop.second << ";" << std::endl;
        }
        unindent();
    }

    void AstPrinter::indent()
    {
        indentation += "  ";
    }

    void AstPrinter::unindent()
    {
        if (indentation.length() >= 2)
        {
            indentation.resize(indentation.length() - 2);
        }
    }
}
