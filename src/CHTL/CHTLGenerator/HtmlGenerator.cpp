#include "HtmlGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"

namespace CHTL
{

    std::string HtmlGenerator::generate(const std::vector<std::unique_ptr<BaseNode>> &nodes)
    {
        output.str(""); // Clear the stream
        output.clear();
        for (const auto &node : nodes)
        {
            node->accept(*this);
        }
        return output.str();
    }

    void HtmlGenerator::visit(ElementNode &node)
    {
        // 1. Pre-process children to find style nodes
        std::string styleString;
        for (const auto &child : node.children)
        {
            // Use dynamic_cast to check if the child is a StyleNode
            if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                for (const auto &prop : styleNode->properties)
                {
                    styleString += prop.first + ": " + prop.second + "; ";
                }
            }
        }
        // Trim trailing space
        if (!styleString.empty()) {
            styleString.pop_back();
        }

        // 2. Write opening tag and attributes
        output << "<" << node.tagName;

        for (const auto &attr : node.attributes)
        {
            output << " " << attr.first << "=\"" << attr.second << "\"";
        }
        if (!styleString.empty()) {
            output << " style=\"" << styleString << "\"";
        }

        output << ">";

        // 3. Recursively visit children that are not style nodes
        for (const auto &child : node.children)
        {
            if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
                child->accept(*this);
            }
        }

        // 4. Write closing tag
        output << "</" << node.tagName << ">";
    }

    void HtmlGenerator::visit(TextNode &node)
    {
        // Note: In a real-world scenario, you would HTML-escape the content here.
        output << node.content;
    }

    void HtmlGenerator::visit(CommentNode &node)
    {
        if (node.type == CommentNode::Type::GENERATOR)
        {
            // The lexer includes the '#' in the lexeme. We should strip it.
            std::string content = node.content;
            if (!content.empty() && content[0] == '#') {
                content = content.substr(1);
                // Also strip leading space if it exists
                if (!content.empty() && content[0] == ' ') {
                    content = content.substr(1);
                }
            }
            output << "<!-- " << content << " -->";
        }
        // Regular comments are ignored, so we do nothing.
    }

    void HtmlGenerator::visit(StyleNode &node)
    {
        // This is intentionally left empty.
        // The parent ElementNode is responsible for processing its StyleNode children.
    }

}
