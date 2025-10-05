#include "CHTLGenerator.h"

CHTLGenerator::CHTLGenerator(const std::vector<std::unique_ptr<Node>>& ast) : ast(ast) {}

std::string CHTLGenerator::generate() {
    std::stringstream ss;
    for (const auto& node : ast) {
        generateNode(ss, node.get());
    }
    return ss.str();
}

void CHTLGenerator::generateNode(std::stringstream& ss, const Node* node) {
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(node)) {
        ss << "<" << elementNode->getTagName();
        for (const auto& attr : elementNode->getAttributes()) {
            ss << " " << attr.first << "=" << attr.second;
        }
        ss << ">";
        for (const auto& child : elementNode->getChildren()) {
            generateNode(ss, child.get());
        }
        ss << "</" << elementNode->getTagName() << ">";
    } else if (const auto* textNode = dynamic_cast<const TextNode*>(node)) {
        std::string text = textNode->getText();
        if (text.front() == '"' && text.back() == '"') {
            ss << text.substr(1, text.length() - 2);
        } else {
            ss << text;
        }
    } else if (const auto* commentNode = dynamic_cast<const CommentNode*>(node)) {
        std::string comment = commentNode->getComment();
        if (comment.rfind("#", 0) == 0) {
            ss << "<!--" << comment.substr(1) << " -->";
        }
    }
}