#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLGenerator::generate(const std::shared_ptr<BaseNode>& node) {
    std::string output;
    if (node) {
        generateNode(node, output);
    }
    return output;
}

void CHTLGenerator::generateNode(const std::shared_ptr<BaseNode>& node, std::string& output) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::NODE_ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            output += "<" + elementNode->getTagName();
            for (const auto& attr : elementNode->getAttributes()) {
                output += " " + attr.first + "=\"" + attr.second + "\"";
            }
            output += ">";
            for (const auto& child : elementNode->getChildren()) {
                generateNode(child, output);
            }
            output += "</" + elementNode->getTagName() + ">";
            break;
        }
        case NodeType::NODE_TEXT: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            output += textNode->getContent();
            break;
        }
        case NodeType::NODE_STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            // Note: This generates an inline style tag. The spec implies some styles
            // should be hoisted to a global style block, which is a future enhancement.
            output += "<style>";
            for (const auto& prop : styleNode->getProperties()) {
                if(prop)
                {
                    output += prop->getKey() + ": " + prop->getValue() + ";";
                }
            }
            output += "</style>";
            break;
        }
        default:
            // For now, we ignore other node types.
            // A more robust implementation would throw an error or handle them.
            break;
    }
}

}