#include "CHTLGenerator.h"
#include "../CHTLNode/CHTLNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

std::string CHTLGenerator::generate(CHTLNode& root) {
    result.clear();
    root.accept(*this);
    return result;
}

void CHTLGenerator::visit(ElementNode& node) {
    result += "<" + node.getTagName() + ">";
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    result += "</" + node.getTagName() + ">";
}

void CHTLGenerator::visit(TextNode& node) {
    // Note: In a real implementation, we would need to escape HTML entities
    // in the text content to prevent XSS vulnerabilities and rendering issues.
    result += node.getContent();
}

void CHTLGenerator::visit(CommentNode& node) {
    // The spec says "# comment content". The lexer captures " comment content".
    // We should probably trim leading/trailing whitespace from the content.
    std::string content = node.getContent();
    size_t first = content.find_first_not_of(" \t");
    if (std::string::npos != first) {
        size_t last = content.find_last_not_of(" \t");
        content = content.substr(first, (last - first + 1));
    } else {
        content = "";
    }

    result += "<!--" + content + "-->";
}
