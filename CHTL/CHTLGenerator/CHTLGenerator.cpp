#include "CHTLGenerator.h"
#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/OriginNode.h"
#include <iostream>

CHTLGenerator::CHTLGenerator(std::shared_ptr<ElementNode> root, CHTLContext& context) : context(context), rootNode(root) {}

std::string CHTLGenerator::generate() {
    output.str("");
    output.clear();

    if (context.useHtml5Doctype) {
        output << "<!DOCTYPE html>\n";
    }

    visit(rootNode);
    return output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  ";
    }
}

void CHTLGenerator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            visitElement(std::dynamic_pointer_cast<ElementNode>(node));
            break;
        case NodeType::Text:
            visitText(std::dynamic_pointer_cast<TextNode>(node));
            break;
        case NodeType::Comment:
            visitComment(std::dynamic_pointer_cast<CommentNode>(node));
            break;
        case NodeType::Origin:
            visitOrigin(std::dynamic_pointer_cast<OriginNode>(node));
            break;
        default:
            break;
    }
}

void CHTLGenerator::visitElement(const std::shared_ptr<ElementNode>& node) {
    if (node->tagName == "__ROOT__") {
        for (const auto& child : node->children) {
            visit(child);
        }
        return;
    }

    indent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    if (!node->finalStyles.empty()) {
        output << " style=\"";
        for (const auto& stylePair : node->finalStyles) {
            output << stylePair.first << ": " << stylePair.second << ";";
        }
        output << "\"";
    }

    if (selfClosingTags.count(node->tagName)) {
        output << " />\n";
        return;
    }

    output << ">\n";
    indentLevel++;

    if (node->tagName == "head" && !context.finalGlobalCssRules.empty()) {
        indent();
        output << "<style>\n";
        indentLevel++;
        for (const auto& rule : context.finalGlobalCssRules) {
            indent();
            output << rule << "\n";
        }
        indentLevel--;
        indent();
        output << "</style>\n";
    }

    for (const auto& child : node->children) {
        visit(child);
    }

    indentLevel--;
    indent();
    output << "</" << node->tagName << ">\n";
}

void CHTLGenerator::visitText(const std::shared_ptr<TextNode>& node) {
    indent();
    output << node->content << "\n";
}

void CHTLGenerator::visitComment(const std::shared_ptr<CommentNode>& node) {
    if (node->isGeneratorAware) {
        indent();
        output << "<!-- " << node->content << " -->\n";
    }
}

void CHTLGenerator::visitOrigin(const std::shared_ptr<OriginNode>& node) {
    // Output raw content exactly as is, without indentation
    output << node->rawContent;
}
