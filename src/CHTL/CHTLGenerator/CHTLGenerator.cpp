#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include <sstream>
#include <vector>
#include <algorithm>

namespace CHTL {

// List of common HTML self-closing tags
const std::vector<std::string> SELF_CLOSING_TAGS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

bool isSelfClosing(const std::string& tagName) {
    return std::find(SELF_CLOSING_TAGS.begin(), SELF_CLOSING_TAGS.end(), tagName) != SELF_CLOSING_TAGS.end();
}

std::string CHTLGenerator::generate(NodePtr& ast) {
    if (!ast) return "";
    return ast->accept(*this);
}

std::string CHTLGenerator::visit(ElementNode& node) {
    // The "root" node is a special case from the parser, we don't render it.
    // We just render its children.
    if (node.tagName == "root") {
        std::stringstream ss;
        for (const auto& child : node.children) {
            ss << child->accept(*this);
        }
        return ss.str();
    }

    std::stringstream ss;
    ss << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        ss << " " << attr->key << "=\"" << attr->value << "\"";
    }

    if (isSelfClosing(node.tagName)) {
        ss << ">";
        return ss.str();
    }

    ss << ">";

    for (const auto& child : node.children) {
        ss << child->accept(*this);
    }

    ss << "</" << node.tagName << ">";
    return ss.str();
}

std::string CHTLGenerator::visit(TextNode& node) {
    // Basic HTML escaping could be added here in a real implementation
    return node.content;
}

std::string CHTLGenerator::visit(CommentNode& node) {
    if (node.commentType == CommentType::Generator) {
        return "<!-- " + node.content + " -->";
    }
    return ""; // Other comments are ignored
}

std::string CHTLGenerator::visit(AttributeNode& node) {
    // This should not be called directly, as attributes are handled
    // by the ElementNode visitor.
    return "";
}

} // namespace CHTL
