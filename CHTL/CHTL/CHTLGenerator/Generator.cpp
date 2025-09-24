#include "Generator.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/StyleNode.h"

std::string Generator::generate(BaseNode& root) {
    // Clear streams for this generation run
    output.str("");
    output.clear();
    globalCss.str("");
    globalCss.clear();

    // Pass 1: Traverse AST and generate HTML body + collect global CSS
    root.accept(*this);
    std::string html = output.str();

    // Pass 2: Inject collected global CSS into the <head>
    if (!globalCss.str().empty()) {
        std::string styleBlock = "<style>\n" + globalCss.str() + "</style>\n";
        size_t head_pos = html.find("</head>");
        if (head_pos != std::string::npos) {
            html.insert(head_pos, styleBlock);
        } else {
            // If no head, prepend to the document. This is a fallback.
            html.insert(0, "<head>\n" + styleBlock + "</head>\n");
        }
    }

    return html;
}


void Generator::visit(ElementNode& node) {
    output << "<" << node.getTagName();

    auto attributes = node.getAttributes();
    std::string inline_style_content;

    // Process the style node if it exists
    if (node.getStyleNode()) {
        const StyleNode* styleNode = node.getStyleNode();

        // 1. Collect inline properties
        for (const auto& prop : styleNode->getInlineProperties()) {
            inline_style_content += prop.first + ": " + prop.second + "; ";
        }
        if (!inline_style_content.empty()) {
            // Trim trailing space
            inline_style_content.pop_back();
            attributes["style"] = inline_style_content;
        }

        // 2. Collect and hoist global rules
        for (const auto& rule : styleNode->getCssRules()) {
            globalCss << "  " << rule.selector << " {\n";
            globalCss << "    " << rule.content << "\n";
            globalCss << "  }\n";
        }
    }

    // Add attributes to the tag
    for (const auto& attr : attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    output << ">";

    // Pretty-printing logic
    bool simpleTextChild = node.getChildren().size() == 1 && dynamic_cast<TextNode*>(node.getChildren().front().get());
    if (!simpleTextChild && !node.getChildren().empty()) {
        output << "\n";
    }

    // Visit children
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    // Pretty-printing logic
    if (!simpleTextChild && !node.getChildren().empty()) {
        output << "\n";
    }

    output << "</" << node.getTagName() << ">" << "\n";
}

void Generator::visit(TextNode& node) {
    output << node.getContent();
}

void Generator::visit(CommentNode& node) {
    output << "<!-- " << node.getContent() << " -->\n";
}
