#include "CHTLGenerator.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLGenerator::generate(const BaseNode& root) {
    visit(&root);

    std::stringstream output;
    if (css_ss.str().length() > 0) {
        output << "<style>\n" << css_ss.str() << "</style>\n";
    }
    output << html_ss.str();

    return output.str();
}

void CHTLGenerator::visit(const BaseNode* node) {
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(node)) {
        visitElementNode(elementNode);
    } else if (const auto* textNode = dynamic_cast<const TextNode*>(node)) {
        visitTextNode(textNode);
    }
}

void CHTLGenerator::visitElementNode(const ElementNode* node) {
    if (node->getStyle()) {
        visitStyleNode(node, node->getStyle());
    }

    html_ss << "<" << node->getTagName();

    for (const auto& attr : node->getAttributes()) {
        html_ss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    html_ss << ">";

    for (const auto& child : node->getChildren()) {
        visit(child.get());
    }

    html_ss << "</" << node->getTagName() << ">";
}

void CHTLGenerator::visitTextNode(const TextNode* node) {
    html_ss << node->getValue();
}

void CHTLGenerator::visitStyleNode(const ElementNode* parent, const StyleNode* styleNode) {
    // Handle CSS rules
    for (const auto& rule : styleNode->getRules()) {
        std::string selector = rule->getSelector();
        if (selector[0] == '.') {
            const_cast<ElementNode*>(parent)->addAttribute("class", selector.substr(1));
        } else if (selector[0] == '#') {
            const_cast<ElementNode*>(parent)->addAttribute("id", selector.substr(1));
        }

        css_ss << rule->getSelector() << " {\n";
        for (const auto& prop : rule->getProperties()) {
            css_ss << "  " << prop->getKey() << ": " << visitExpression(prop->getValue()) << ";\n";
        }
        css_ss << "}\n";
    }

    // Handle inline properties
    if (!styleNode->getProperties().empty()) {
        std::stringstream inline_style_ss;
        for (const auto& prop : styleNode->getProperties()) {
            inline_style_ss << prop->getKey() << ":" << visitExpression(prop->getValue()) << ";";
        }
        const_cast<ElementNode*>(parent)->addAttribute("style", inline_style_ss.str());
    }
}

std::string CHTLGenerator::visitExpression(const ExpressionNode* node) {
    if (const auto* literalNode = dynamic_cast<const LiteralNode*>(node)) {
        return literalNode->getValue().value;
    }
    if (const auto* binaryOpNode = dynamic_cast<const BinaryOpNode*>(node)) {
        std::string left = visitExpression(binaryOpNode->getLeft());
        std::string right = visitExpression(binaryOpNode->getRight());
        return left + " " + binaryOpNode->getOperator().value + " " + right;
    }
    return "";
}

} // namespace CHTL