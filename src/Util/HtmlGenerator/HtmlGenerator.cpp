#include "HtmlGenerator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include "../../CHTL/CHTLNode/IfBehaviorNode.h"
#include <sstream>
#include <vector>

std::string value_to_string(const BaseNode* node) {
    if (auto* val = dynamic_cast<const ValueNode*>(node)) {
        return val->getValue();
    }
    return "";
}

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

            // Render attributes other than style, which we will build manually.
            for (const auto& attr : elementNode->getAttributes()) {
                if (attr.first != "style") {
                    html += " " + attr.first + "=\"" + attr.second + "\"";
                }
            }

            // Collect all style properties from all StyleNode children.
            std::stringstream style_ss;
            for (const auto& child : elementNode->getChildren()) {
                if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                    for (const auto& style_child : styleNode->getChildren()) {
                        if (auto* prop = dynamic_cast<StylePropertyNode*>(style_child.get())) {
                            style_ss << prop->getName() << ": " << value_to_string(prop->getValue()) << "; ";
                        }
                    }
                }
            }

            std::string style_str = style_ss.str();
            if (!style_str.empty()) {
                // Remove the trailing space
                if (style_str.back() == ' ') {
                    style_str.pop_back();
                }
                html += " style=\"" + style_str + "\"";
            }

            html += ">\n";

            // Traverse children that are not style-related.
            for (const auto& child : elementNode->getChildren()) {
                if (child->getType() != NodeType::Style && child->getType() != NodeType::IfBehavior) {
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
        case NodeType::IfBehavior:
            // No-op, this is handled by the StyleEvaluator.
            break;
        default:
            for (const auto& child : node->getChildren()) {
                traverse(child.get(), html, indent);
            }
            break;
    }
}