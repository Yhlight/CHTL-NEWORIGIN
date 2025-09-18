#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>

namespace CHTL {

// List of common HTML self-closing tags
const std::vector<std::string> SELF_CLOSING_TAGS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

bool isSelfClosing(const std::string& tagName) {
    return std::find(SELF_CLOSING_TAGS.begin(), SELF_CLOSING_TAGS.end(), tagName) != SELF_CLOSING_TAGS.end();
}

GeneratedResult CHTLGenerator::generate(NodePtr& ast) {
    GeneratedResult result;
    if (!ast) return result;
    result.html = ast->accept(*this);
    result.css = global_css.str();
    return result;
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
    std::string element_id;
    std::set<std::string> element_classes;
    std::vector<StyleProperty> inline_properties;

    // First, find explicit id and class from attributes
    for (const auto& attr : node.attributes) {
        if (attr->key == "id") {
            element_id = attr->value;
        } else if (attr->key == "class") {
            // Simple split by space for multiple classes
            std::stringstream class_stream(attr->value);
            std::string class_name;
            while (class_stream >> class_name) {
                element_classes.insert(class_name);
            }
        }
    }

    // Collect styles and auto-add classes/ids from child StyleNodes
    for (const auto& child : node.children) {
        if (child->getType() == NodeType::Style) {
            StyleNode* style_node = static_cast<StyleNode*>(child.get());
            inline_properties.insert(inline_properties.end(), style_node->inline_properties.begin(), style_node->inline_properties.end());

            for (const auto& rule : style_node->nested_rules) {
                std::string selector = rule.selector;
                if (selector.rfind('.', 0) == 0) { // starts with .
                    element_classes.insert(selector.substr(1));
                } else if (selector.rfind('#', 0) == 0) { // starts with #
                    element_id = selector.substr(1);
                }

                // For now, just add the rule to global css. More complex selectors need more logic.
                global_css << selector << " {\n";
                for (const auto& prop : rule.properties) {
                    global_css << "    " << prop.key << ": " << prop.value << ";\n";
                }
                global_css << "}\n";
            }
        }
    }

    // Build the element tag
    ss << "<" << node.tagName;

    if (!element_id.empty()) {
        ss << " id=\"" << element_id << "\"";
    }

    if (!element_classes.empty()) {
        ss << " class=\"";
        for (auto it = element_classes.begin(); it != element_classes.end(); ++it) {
            if (it != element_classes.begin()) ss << " ";
            ss << *it;
        }
        ss << "\"";
    }

    // Print other explicit attributes that are not id or class
    for (const auto& attr : node.attributes) {
        if (attr->key != "id" && attr->key != "class") {
            ss << " " << attr->key << "=\"" << attr->value << "\"";
        }
    }

    if (!inline_properties.empty()) {
        ss << " style=\"";
        for (const auto& prop : inline_properties) {
            ss << prop.key << ": " << prop.value << ";";
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
    return "";
}

} // namespace CHTL
