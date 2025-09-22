#include "GlobalStyleVisitor.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/CssRuleNode.h"

namespace CHTL {

void GlobalStyleVisitor::collect(BaseNode* root) {
    collectedRules.clear();
    if (root) {
        root->accept(*this);
    }
}

const std::vector<CssRuleNode*>& GlobalStyleVisitor::getCollectedRules() const {
    return collectedRules;
}

void GlobalStyleVisitor::visit(ElementNode& node) {
    // Recursively visit children to find style blocks and rules
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void GlobalStyleVisitor::visit(StyleBlockNode& node) {
    // A style block can contain rules, so we visit its children
    for (const auto& rule : node.rules) {
        rule->accept(*this);
    }
}

void GlobalStyleVisitor::visit(CssRuleNode& node) {
    // This is what we are looking for. Collect it.
    collectedRules.push_back(&node);
}

// These nodes do not contain style rules, so their visit methods do nothing.
void GlobalStyleVisitor::visit(TextNode& node) {}
void GlobalStyleVisitor::visit(AttributeNode& node) {}
void GlobalStyleVisitor::visit(CssPropertyNode& node) {}


} // namespace CHTL
