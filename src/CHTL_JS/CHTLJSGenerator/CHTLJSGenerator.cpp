#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
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
            last_expression.clear();
        } else if (node->getType() == CHTLJSNodeType::EventBinding) {
            if (last_expression.empty()) {
                throw std::runtime_error("Event binding operator must be preceded by an enhanced selector.");
            }
            const auto* bindingNode = static_cast<const EventBindingNode*>(node.get());
            for (const auto& event_name : bindingNode->event_names) {
                final_code << last_expression << ".addEventListener('" << event_name << "', " << bindingNode->handler_code << ");\n";
            }
            last_expression.clear();
        } else if (node->getType() == CHTLJSNodeType::Delegate) {
            if (last_expression.empty()) {
                throw std::runtime_error("Delegate block must be preceded by an enhanced selector.");
            }
            const auto* delegateNode = static_cast<const DelegateNode*>(node.get());
            std::string parent_element_var = "parent_for_delegation_" + std::to_string(reinterpret_cast<uintptr_t>(delegateNode));
            final_code << "const " << parent_element_var << " = " << last_expression << ";\n";

            for (const auto& pair : delegateNode->event_handlers) {
                final_code << parent_element_var << ".addEventListener('" << pair.first << "', (event) => {\n";
                final_code << "  let target = event.target;\n";
                final_code << "  while (target && target !== " << parent_element_var << ") {\n";

                std::string condition;
                for(size_t i = 0; i < delegateNode->target_selectors.size(); ++i) {
                    if (i > 0) condition += " || ";
                    condition += "target.matches('" + delegateNode->target_selectors[i] + "')";
                }

                final_code << "    if (" << condition << ") {\n";
                final_code << "      (" << pair.second << ").call(target, event);\n";
                final_code << "      break;\n";
                final_code << "    }\n";
                final_code << "    target = target.parentNode;\n";
                final_code << "  }\n";
                final_code << "});\n";
            }
            last_expression.clear();
        }
        else {
            if (!last_expression.empty()) {
                final_code << last_expression << ";\n";
                last_expression.clear();
            }

            std::string current_code = generateNode(node.get());

            if (node->getType() == CHTLJSNodeType::EnhancedSelector) {
                last_expression = current_code;
            } else {
                final_code << current_code;
            }
        }
    }

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
        case CHTLJSNodeType::Delegate:
            return "";
        default:
            return "";
    }
}