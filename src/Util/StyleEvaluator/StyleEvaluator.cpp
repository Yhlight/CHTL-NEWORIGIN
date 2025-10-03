#include "StyleEvaluator.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"

void StyleEvaluator::evaluate(BaseNode* root) {
    if (!root) return;
    // The traverse method needs the root for context, but we don't want to
    // pass it down with every recursive call. Instead, we'll store it as a member.
    this->astRoot = root;
    traverse(root);
}

void StyleEvaluator::traverse(BaseNode* node) {
    if (!node) return;

    if (node->getType() == NodeType::StyleProperty) {
        auto* propNode = static_cast<StylePropertyNode*>(node);

        // Only evaluate if the value is not already a simple ValueNode.
        // This prevents re-evaluating an already resolved expression.
        if (propNode->getValue() && propNode->getValue()->getType() != NodeType::Value) {
            std::string finalValue = evaluator.evaluate(propNode->getValue(), this->astRoot);
            propNode->setValue(std::make_unique<ValueNode>(finalValue));
        }
    }

    for (const auto& child : node->getChildren()) {
        traverse(child.get());
    }
}