#include "HtmlGenerator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include <sstream>

std::string HtmlGenerator::generate(BaseNode* root) {
    std::string html;
    traverse(root, html);
    return html;
}

void HtmlGenerator::traverse(BaseNode* node, std::string& html, int indent) {
    if (!node) return;

    std::string indentation(indent * 2, ' ');

    switch (node->getType()) {
        case NodeType::Element: {
            auto* elementNode = static_cast<ElementNode*>(node);
            html += indentation + "<" + elementNode->getTagName();

            std::string styleString;
            for (const auto& child : elementNode->getChildren()) {
                if (child->getType() == NodeType::Style) {
                    auto* styleNode = static_cast<StyleNode*>(child.get());
                    for (const auto& styleChild : styleNode->getChildren()) {
                        if (styleChild->getType() == NodeType::StyleProperty) {
                            auto* prop = static_cast<StylePropertyNode*>(styleChild.get());
                            if (prop->getValue()->getType() == NodeType::Value) {
                                const auto* val = static_cast<const ValueNode*>(prop->getValue());
                                styleString += prop->getName() + ": " + val->getValue() + ";";
                            }
                        }
                    }
                }
            }

            for (const auto& attr : elementNode->getAttributes()) {
                html += " " + attr.first + "=\"" + attr.second + "\"";
            }

            if (!styleString.empty()) {
                 html += " style=\"" + styleString + "\"";
            }

            html += ">\n";

            for (const auto& child : elementNode->getChildren()) {
                if(child->getType() != NodeType::Style) {
                    traverse(child.get(), html, indent + 1);
                }
            }
            html += indentation + "</" + elementNode->getTagName() + ">\n";
            break;
        }
        case NodeType::Text: {
            html += indentation + static_cast<const TextNode*>(node)->getText() + "\n";
            break;
        }
        case NodeType::Comment: {
            html += indentation + "<!-- " + static_cast<const CommentNode*>(node)->getComment() + " -->\n";
            break;
        }
        default:
            for (const auto& child : node->getChildren()) {
                traverse(child.get(), html, indent);
            }
            break;
    }
}