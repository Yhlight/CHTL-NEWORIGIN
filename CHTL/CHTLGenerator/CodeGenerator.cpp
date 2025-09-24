#include "CodeGenerator.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

std::string CodeGenerator::Generate(Node* root, Context& context) {
    // First, generate the HTML from the AST
    if (root) {
        root->Accept(*this);
    }

    // Then, prepend the global CSS block
    if (!context.globalCss.empty()) {
        std::stringstream finalOutput;
        finalOutput << "<style>" << context.globalCss << "</style>\n";
        finalOutput << output.str();
        return finalOutput.str();
    }

    return output.str();
}

void CodeGenerator::Visit(ElementNode& node) {
    if (node.tagName == "__ROOT__") {
        for (const auto& child : node.children) {
            child->Accept(*this);
        }
        return;
    }

    // Generate the opening tag with attributes
    output << "<" << node.tagName;
    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }
    output << ">";

    // Visit all children
    for (const auto& child : node.children) {
        child->Accept(*this);
    }

    // Generate the closing tag
    output << "</" << node.tagName << ">";
}

void CodeGenerator::Visit(TextNode& node) {
    output << node.text;
}

void CodeGenerator::Visit(CommentNode& node) {
    output << "<!-- " << node.text << " -->";
}

} // namespace CHTL
