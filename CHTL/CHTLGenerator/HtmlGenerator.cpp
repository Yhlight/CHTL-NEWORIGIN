#include "HtmlGenerator.h"
#include <vector>

// Helper to get inline style string from a StyleNode
std::string getInlineStyle(const StyleNode* styleNode) {
    if (!styleNode) return "";
    std::stringstream ss;
    const auto& properties = styleNode->getProperties();
    for (size_t i = 0; i < properties.size(); ++i) {
        ss << properties[i].first << ": " << properties[i].second << ";";
        if (i < properties.size() - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

std::string HtmlGenerator::getResult() {
    return resultStream.str();
}

void HtmlGenerator::visit(DocumentNode& node) {
    if (node.getHasHtml5Doctype()) {
        resultStream << "<!DOCTYPE html>\n";
    }
    resultStream << "<html>\n";

    // First pass: collect all CSS rules for hoisting
    std::vector<StyleNode*> styleNodes;
    findStyleNodes(&node, styleNodes);
    for (auto* styleNode : styleNodes) {
        styleNode->accept(*this);
    }

    // Generate head with hoisted styles
    resultStream << "<head>\n";
    if (!hoistedCss.str().empty()) {
        resultStream << "  <style>\n" << hoistedCss.str() << "  </style>\n";
    }
    resultStream << "</head>\n";

    // Generate body
    resultStream << "<body>\n";
    depth++;
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    depth--;
    resultStream << "</body>\n";

    resultStream << "</html>\n";
}

void HtmlGenerator::visit(ElementNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << "<" << node.getTagName();

    // Find associated style node to extract inline styles
    const StyleNode* styleNode = nullptr;
    for (const auto& child : node.getChildren()) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            styleNode = sn;
            break;
        }
    }

    if (styleNode) {
        std::string inlineStyle = getInlineStyle(styleNode);
        if (!inlineStyle.empty()) {
            resultStream << " style=\"" << inlineStyle << "\"";
        }
    }

    resultStream << ">\n";
    depth++;
    for (const auto& child : node.getChildren()) {
        // StyleNodes are processed separately, so we skip them here
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            child->accept(*this);
        }
    }
    depth--;
    resultStream << indent << "</" << node.getTagName() << ">\n";
}

void HtmlGenerator::visit(TextNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << node.toString(0); // Use existing simple to string for content
}

void HtmlGenerator::visit(CommentNode& node) {
    std::string indent(depth * 2, ' ');
    resultStream << indent << node.toString(0); // Use existing simple to string for content
}

void HtmlGenerator::visit(StyleNode& node) {
    // This visit is for hoisting CSS rules. Inline styles are handled by ElementNode visitor.
    for (const auto& rule : node.getRules()) {
        rule->accept(*this);
    }
}

void HtmlGenerator::visit(CssRuleNode& node) {
    std::string indent(depth * 2, ' ');
    hoistedCss << indent << "  " << node.getSelector() << " {\n";
    for (const auto& prop : node.getProperties()) {
        hoistedCss << indent << "    " << prop.first << ": " << prop.second << ";\n";
    }
    hoistedCss << indent << "  }\n";
}

void HtmlGenerator::findStyleNodes(BaseNode* node, std::vector<StyleNode*>& styleNodes) {
    if (!node) return;

    if (auto* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) {
            if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                styleNodes.push_back(styleNode);
            }
            // Recurse into children of elements
            findStyleNodes(child.get(), styleNodes);
        }
    } else if (auto* docNode = dynamic_cast<DocumentNode*>(node)) {
        for (const auto& child : docNode->getChildren()) {
            findStyleNodes(child.get(), styleNodes);
        }
    }
    // Other node types (like NamespaceNode) could be handled here if they can contain elements
}