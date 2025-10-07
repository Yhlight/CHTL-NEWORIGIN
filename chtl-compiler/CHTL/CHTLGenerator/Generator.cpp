#include "chtl-compiler/CHTL/CHTLGenerator/Generator.h"
#include "chtl-compiler/CHTL/CHTLNode/ElementNode.h"
#include "chtl-compiler/CHTL/CHTLNode/TextNode.h"
#include <stdexcept>
#include <sstream>

std::string Generator::generate(const NodeList& nodes) {
    std::stringstream output;
    for (const auto& node : nodes) {
        visit(node, output);
    }
    return output.str();
}

std::string Generator::generate(const NodePtr& node) {
    std::stringstream output;
    visit(node, output);
    return output.str();
}

void Generator::visit(const NodePtr& node, std::stringstream& output) {
    if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        output << "<" << elementNode->tagName;

        for (const auto& attr : elementNode->attributes) {
            output << " " << attr.first << "=\"" << attr.second << "\"";
        }

        output << ">";

        for (const auto& child : elementNode->children) {
            visit(child, output);
        }
        output << "</" << elementNode->tagName << ">";
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        output << textNode->content;
    } else {
        // For now, we don't handle other node types
    }
}
