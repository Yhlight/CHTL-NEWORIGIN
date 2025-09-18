#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>

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
    if (node.tagName == "root") {
        std::stringstream ss;
        for (const auto& child : node.children) {
            ss << child->accept(*this);
        }
        return ss.str();
    }

    std::stringstream ss;
    ss << "<" << node.tagName;

    // Print explicit attributes
    for (const auto& attr : node.attributes) {
        ss << " " << attr->key << "=\"" << attr->value << "\"";
    }

    // Collect styles from any child StyleNodes
    std::map<std::string, std::string> inline_styles;
    for (const auto& child : node.children) {
        if (child->getType() == NodeType::Style) {
            StyleNode* style_node = static_cast<StyleNode*>(child.get());
            for (const auto& prop : style_node->properties) {
                inline_styles[prop.first] = prop.second;
            }
        }
    }

    // Print collected inline styles
    if (!inline_styles.empty()) {
        ss << " style=\"";
        for (auto it = inline_styles.begin(); it != inline_styles.end(); ++it) {
            ss << it->first << ": " << it->second << ";";
        }
        ss << "\"";
    }

    if (isSelfClosing(node.tagName)) {
        ss << ">";
        return ss.str();
    }

    ss << ">";

    // Render children that are not style nodes
    for (const auto& child : node.children) {
        if (child->getType() != NodeType::Style) {
            ss << child->accept(*this);
        }
    }

    ss << "</" << node.tagName << ">";
    return ss.str();
}

std::string CHTLGenerator::visit(TextNode& node) {
    return node.content;
}

std::string CHTLGenerator::visit(CommentNode& node) {
    if (node.commentType == CommentType::Generator) {
        return "<!-- " + node.content + " -->";
    }
    return "";
}

std::string CHTLGenerator::visit(AttributeNode& node) {
    return "";
}

std::string CHTLGenerator::visit(StyleNode& node) {
    // StyleNodes are handled by their parent ElementNode, so they generate no output themselves.
    return "";
}

} // namespace CHTL
