#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include <sstream>
#include <stdexcept>

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJSNode>>& ast) {
    std::stringstream final_code;
    std::string last_expression;

    for (const auto& node : ast) {
        if (node->getType() == CHTLJSNodeType::Listen) {
            if (last_expression.empty()) {
                throw std::runtime_error("Listen block must be preceded by an enhanced selector.");
            }
            const auto* listenNode = static_cast<const ListenNode*>(node.get());
            for (const auto& pair : listenNode->event_handlers) {
                final_code << last_expression << ".addEventListener('" << pair.first << "', " << pair.second << ");\n";
            }
            last_expression.clear(); // The expression has been consumed.
        } else if (node->getType() == CHTLJSNodeType::EventBinding) {
            if (last_expression.empty()) {
                throw std::runtime_error("Event binding operator must be preceded by an enhanced selector.");
            }
            const auto* bindingNode = static_cast<const EventBindingNode*>(node.get());
            for (const auto& event_name : bindingNode->event_names) {
                final_code << last_expression << ".addEventListener('" << event_name << "', " << bindingNode->handler_code << ");\n";
            }
            // For now, assume the expression is consumed. Chaining can be a future enhancement.
            last_expression.clear();
        } else {
            // If there was a pending expression, it means it wasn't used by a function call.
            // So, append it to the output as a standalone statement.
            if (!last_expression.empty()) {
                final_code << last_expression << ";\n";
                last_expression.clear();
            }

            // Generate the code for the current node.
            std::string current_code = generateNode(node.get());

            // If it's a selector, it's an expression that might be used by the next node.
            // Otherwise, it's a statement that should be appended immediately.
            if (node->getType() == CHTLJSNodeType::EnhancedSelector) {
                last_expression = current_code;
            } else {
                final_code << current_code;
            }
        }
    }

    // Append any leftover expression.
    if (!last_expression.empty()) {
        final_code << last_expression << ";\n";
    }

    return final_code.str();
}

std::string CHTLJSGenerator::generateNode(const CHTLJSNode* node) {
    if (!node) return "";

    switch (node->getType()) {
        case CHTLJSNodeType::RawJavaScript: {
            return static_cast<const RawJavaScriptNode*>(node)->js_code;
        }
        case CHTLJSNodeType::EnhancedSelector: {
            return "document.querySelector('" + static_cast<const CHTLJSEnhancedSelectorNode*>(node)->selector_text + "')";
        }
        case CHTLJSNodeType::Listen:
        case CHTLJSNodeType::EventBinding:
            // This is handled in the main generate loop, so we return an empty string here.
            return "";
        default:
            return "";
    }
}