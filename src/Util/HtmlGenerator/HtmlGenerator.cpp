#include "HtmlGenerator.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/OriginNode.h"
#include "../../CHTL/CHTLNode/DynamicBlockNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include <sstream>

std::string HtmlGenerator::generate(const BaseNode* root) {
    std::string output;
    traverse(root, output);
    return output;
}

std::string HtmlGenerator::generateIndent(int depth) {
    return std::string(depth * 2, ' ');
}

void HtmlGenerator::traverse(const BaseNode* node, std::string& output, int depth) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element: {
            const auto* elementNode = static_cast<const ElementNode*>(node);
            output += generateIndent(depth) + "<" + elementNode->getTagName();
            for (const auto& attr : elementNode->getAttributes()) {
                output += " " + attr.first + "=\"" + attr.second + "\"";
            }
            output += ">\n";

            for (const auto& child : node->getChildren()) {
                traverse(child.get(), output, depth + 1);
            }

            output += generateIndent(depth) + "</" + elementNode->getTagName() + ">\n";
            break;
        }
        case NodeType::Text: {
            const auto* textNode = static_cast<const TextNode*>(node);
            output += generateIndent(depth) + textNode->getText() + "\n";
            break;
        }
        case NodeType::Origin: {
             const auto* originNode = static_cast<const OriginNode*>(node);
             // Only generate content for @Html origin blocks
             if (originNode->getOriginType() == "@Html") {
                output += generateIndent(depth) + originNode->getContent() + "\n";
             }
            break;
        }
        case NodeType::DynamicBlock: {
            const auto* dynamicNode = static_cast<const DynamicBlockNode*>(node);
            std::stringstream stylesJson;
            stylesJson << "{";
            bool first = true;
            for (const auto& child : dynamicNode->getChildren()) {
                if (child->getType() == NodeType::StyleProperty) {
                    if (!first) stylesJson << ",";
                    const auto* prop = static_cast<const StylePropertyNode*>(child.get());
                    const auto* val = static_cast<const ValueNode*>(prop->getValue());
                    stylesJson << "\"" << prop->getName() << "\":\"" << val->getValue() << "\"";
                    first = false;
                }
            }
            stylesJson << "}";

            output += generateIndent(depth) + "<template";
            output += " data-chtl-dynamic-if=\"" + dynamicNode->getCondition() + "\"";
            output += " data-chtl-styles='" + stylesJson.str() + "'";
            output += "></template>\n";
            break;
        }
        default: {
            // For other nodes (like Root, Style, etc.), just traverse their children
            for (const auto& child : node->getChildren()) {
                traverse(child.get(), output, depth);
            }
            break;
        }
    }
}