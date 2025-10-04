#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/SelectorNode.h"
#include "../CHTLJSNode/StringLiteralNode.h"
#include <stdexcept>

namespace CHTLJS {

std::string CHTLJSGenerator::generate(const std::shared_ptr<BaseNode>& node) {
    if (node) {
        visit(node);
    }
    return js_out.str();
}

void CHTLJSGenerator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::NODE_SELECTOR:
            visit(std::dynamic_pointer_cast<SelectorNode>(node));
            break;
        case NodeType::NODE_STRING_LITERAL:
            visit(std::dynamic_pointer_cast<StringLiteralNode>(node));
            break;
        default:
            break;
    }
}

void CHTLJSGenerator::visit(const std::shared_ptr<SelectorNode>& node) {
    js_out << "document.querySelector('" << node->getSelector() << "')";
}

void CHTLJSGenerator::visit(const std::shared_ptr<StringLiteralNode>& node) {
    js_out << "\"" << node->getValue() << "\"";
}

}