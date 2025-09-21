#include "Generator.h"
#include <stdexcept>

namespace CHTL {

std::string Generator::Generate(const Program* program) {
    std::stringstream ss;
    for (const auto& child : program->children) {
        generateNode(child.get(), ss);
    }
    return ss.str();
}

void Generator::generateNode(const Node* node, std::stringstream& ss) {
    switch (node->GetType()) {
        case NodeType::ELEMENT_NODE:
            generateElement(static_cast<const ElementNode*>(node), ss);
            break;
        case NodeType::TEXT_NODE:
            generateText(static_cast<const TextNode*>(node), ss);
            break;
        case NodeType::PROGRAM:
            // Should not happen inside generateNode, but handle defensively
            for (const auto& child : static_cast<const Program*>(node)->children) {
                generateNode(child.get(), ss);
            }
            break;
        default:
            // Handle unknown node types if necessary
            break;
    }
}

void Generator::generateElement(const ElementNode* elementNode, std::stringstream& ss) {
    ss << "<" << elementNode->tagName << ">";

    for (const auto& child : elementNode->children) {
        generateNode(child.get(), ss);
    }

    ss << "</" << elementNode->tagName << ">";
}

void Generator::generateText(const TextNode* textNode, std::stringstream& ss) {
    ss << textNode->value;
}

} // namespace CHTL
