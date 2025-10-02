#include "HtmlGenerator.h"
#include <iostream>

HtmlGenerator::HtmlGenerator(std::unique_ptr<DocumentNode> doc) : document(std::move(doc)) {}

std::string HtmlGenerator::generate() {
    if (!document) return "";

    std::stringstream ss;
    if (document->getHasHtml5Doctype()) {
        ss << "<!DOCTYPE html>\n";
    }

    ss << "<html>\n";

    std::vector<const StyleNode*> allStyleNodes;
    findStyleNodes(document.get(), allStyleNodes);

    ss << generateHead(allStyleNodes);
    ss << generateBody(document->getChildren(), 0);

    ss << "</html>\n";

    return ss.str();
}

void HtmlGenerator::findStyleNodes(BaseNode* node, std::vector<const StyleNode*>& styleNodes) {
    if (!node) return;

    if (auto* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->getChildren()) {
            if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                styleNodes.push_back(styleNode);
            }
            findStyleNodes(child.get(), styleNodes);
        }
    } else if (auto* docNode = dynamic_cast<DocumentNode*>(node)) {
        for (const auto& child : docNode->getChildren()) {
            findStyleNodes(child.get(), styleNodes);
        }
    }
}

std::string HtmlGenerator::generateHead(const std::vector<const StyleNode*>& allStyleNodes) {
    std::stringstream ss;
    ss << "<head>\n";
    ss << "  <style>\n";

    for (const auto* styleNode : allStyleNodes) {
        for (const auto& rule : styleNode->getRules()) {
            ss << "    " << rule->getSelector() << " {\n";
            for (const auto& prop : rule->getProperties()) {
                ss << "      " << prop.first << ": " << prop.second << ";\n";
            }
            ss << "    }\n";
        }
    }

    ss << "  </style>\n";
    ss << "</head>\n";
    return ss.str();
}

std::string HtmlGenerator::generateBody(const std::vector<std::unique_ptr<BaseNode>>& nodes, int depth) {
    std::stringstream ss;
    ss << "<body>\n";
    for (const auto& node : nodes) {
        ss << generateNode(node.get(), depth + 1);
    }
    ss << "</body>\n";
    return ss.str();
}

std::string HtmlGenerator::generateNode(const BaseNode* node, int depth) {
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(node)) {
        return generateElement(elementNode, depth);
    }
    if (const auto* textNode = dynamic_cast<const TextNode*>(node)) {
        return std::string(depth * 2, ' ') + textNode->toString(0);
    }
    if (const auto* commentNode = dynamic_cast<const CommentNode*>(node)) {
        return std::string(depth * 2, ' ') + commentNode->toString(0);
    }
    return "";
}

std::string HtmlGenerator::generateElement(const ElementNode* node, int depth) {
    std::stringstream ss;
    std::string indent(depth * 2, ' ');
    ss << indent << "<" << node->getTagName();

    const StyleNode* styleNode = nullptr;
    for (const auto& child : node->getChildren()) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            styleNode = sn;
            break;
        }
    }

    if (styleNode && !styleNode->getProperties().empty()) {
        ss << " style=\"" << generateInlineStyle(styleNode) << "\"";
    }

    ss << ">\n";

    for (const auto& child : node->getChildren()) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            ss << generateNode(child.get(), depth + 1);
        }
    }

    ss << indent << "</" << node->getTagName() << ">\n";
    return ss.str();
}

std::string HtmlGenerator::generateInlineStyle(const StyleNode* styleNode) const {
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