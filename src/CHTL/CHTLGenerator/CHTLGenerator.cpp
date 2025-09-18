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

std::string buildBaseSelector(ElementNode& node, const std::string& element_id, const std::set<std::string>& element_classes) {
    std::stringstream selector_ss;
    selector_ss << node.tagName;
    if (!element_id.empty()) {
        selector_ss << "#" << element_id;
    }
    for (const auto& cls : element_classes) {
        selector_ss << "." << cls;
    }
    return selector_ss.str();
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
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

    for (const auto& attr : node.attributes) {
        if (attr->key == "id") {
            element_id = attr->value;
        } else if (attr->key == "class") {
            std::stringstream class_stream(attr->value);
            std::string class_name;
            while (class_stream >> class_name) {
                element_classes.insert(class_name);
            }
        }
    }

    for (const auto& child : node.children) {
        if (child->getType() == NodeType::Style) {
            StyleNode* style_node = static_cast<StyleNode*>(child.get());
            inline_properties.insert(inline_properties.end(), style_node->inline_properties.begin(), style_node->inline_properties.end());

            for (const auto& rule : style_node->nested_rules) {
                std::string selector = rule.selector;
                if (selector.rfind('.', 0) == 0) {
                    element_classes.insert(selector.substr(1));
                } else if (selector.rfind('#', 0) == 0) {
                    element_id = selector.substr(1);
                }

                std::string base_selector = buildBaseSelector(node, element_id, element_classes);
                replaceAll(selector, "&", base_selector);

                global_css << selector << " {\n";
                for (const auto& prop : rule.properties) {
                    global_css << "    " << prop.key << ": " << prop.value << ";\n";
                }
                global_css << "}\n";
            }
        }
    }

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
