#include "Generator.h"
#include <stdexcept>

namespace CHTL {

std::string Generator::Generate(const ElementNode& root) {
    std::stringstream ss;
    m_global_css.str("");

    for (const auto& child : root.children) {
        generateNode(*child, ss);
    }

    std::string html = ss.str();
    std::string styles = m_global_css.str();

    if (!styles.empty()) {
        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            std::string style_tag = "<style>" + styles + "</style>";
            html.insert(head_pos, style_tag);
        } else {
            html = "<style>" + styles + "</style>" + html;
        }
    }

    return html;
}

void Generator::generateNode(const BaseNode& node, std::stringstream& ss) {
    if (const auto* element = dynamic_cast<const ElementNode*>(&node)) {
        generateNode(*element, ss);
    } else if (const auto* text = dynamic_cast<const TextNode*>(&node)) {
        generateNode(*text, ss);
    } else if (const auto* style = dynamic_cast<const StyleNode*>(&node)) {
        generateNode(*style, ss);
    }
    else {
        // This case can be reached for nodes that don't produce output, like comments.
        // We can just ignore them.
    }
}

void Generator::generateNode(const ElementNode& element, std::stringstream& ss) {
    if (element.tagName == "__ROOT__") {
        for (const auto& child : element.children) {
            generateNode(*child, ss);
        }
        return;
    }

    ss << "<" << element.tagName;

    for (const auto& attr : element.attributes) {
        ss << " " << attr->name << "=\"" << attr->value << "\"";
    }

    ss << ">";

    for (const auto& child : element.children) {
        generateNode(*child, ss);
    }

    ss << "</" << element.tagName << ">";
}

void Generator::generateNode(const TextNode& text, std::stringstream& ss) {
    ss << text.content;
}

void Generator::generateNode(const StyleNode& style, std::stringstream& ss) {
    m_global_css << style.content;
}

} // CHTL
