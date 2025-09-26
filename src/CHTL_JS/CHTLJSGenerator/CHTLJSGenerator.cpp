#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/RawJavaScriptNode.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/VirtualObjectAccessNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include <sstream>
#include <stdexcept>

std::string CHTLJSGenerator::generate(const std::vector<std::unique_ptr<CHTLJSNode>>& ast, const VirtualObjectManager& vom) {
    this->vom = &vom;
    std::stringstream final_code;
    std::string last_expression;

    scriptLoaderInjected = false;

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

std::string CHTLJSGenerator::generateScriptLoader(const ScriptLoaderNode* node) {
    std::stringstream ss;
    if (!scriptLoaderInjected) {
        ss << R"javascript(
((window) => {
    if (window.ScriptLoader) return;
    const ScriptLoader = {
        load: function(...paths) {
            const promises = paths.map(path => {
                return new Promise((resolve, reject) => {
                    const script = document.createElement('script');
                    script.src = path;
                    script.async = true;
                    script.onload = () => resolve(path);
                    script.onerror = () => reject(new Error(`Failed to load script: ${path}`));
                    document.head.appendChild(script);
                });
            });
            return Promise.all(promises);
        }
    };
    window.ScriptLoader = ScriptLoader;
})(window);
)javascript";
        ss << "\n\n";
        scriptLoaderInjected = true;
    }

    ss << "ScriptLoader.load(";
    for (size_t i = 0; i < node->paths.size(); ++i) {
        ss << "'" << node->paths[i] << "'";
        if (i < node->paths.size() - 1) {
            ss << ", ";
        }
    }
    ss << ");\n";

    return ss.str();
}

std::string CHTLJSGenerator::generateNode(const CHTLJSNode* node) {
    if (!node) return "";

    switch (node->getType()) {
        case CHTLJSNodeType::ScriptLoader:
            return generateScriptLoader(static_cast<const ScriptLoaderNode*>(node));
        case CHTLJSNodeType::RawJavaScript: {
            return static_cast<const RawJavaScriptNode*>(node)->js_code;
        }
        case CHTLJSNodeType::EnhancedSelector: {
            return "document.querySelector('" + static_cast<const CHTLJSEnhancedSelectorNode*>(node)->selector_text + "')";
        }
        case CHTLJSNodeType::VirtualObjectAccess: {
            const auto* accessNode = static_cast<const VirtualObjectAccessNode*>(node);
            const auto* virNode = vom->getVirtualObject(accessNode->objectName);
            if (!virNode) {
                throw std::runtime_error("Virtual object '" + accessNode->objectName + "' not found.");
            }

            const auto* valueNode = virNode->value.get();
            if (valueNode->getType() == CHTLJSNodeType::Listen) {
                const auto* listenNode = static_cast<const ListenNode*>(valueNode);
                auto it = listenNode->event_handlers.find(accessNode->propertyName);
                if (it != listenNode->event_handlers.end()) {
                    return it->second;
                }
            }
            // Add other cases for Animate, etc. here in the future.
            throw std::runtime_error("Property '" + accessNode->propertyName + "' not found in virtual object '" + accessNode->objectName + "'.");
        }
        case CHTLJSNodeType::Animate: {
            const auto* animateNode = static_cast<const AnimateNode*>(node);
            std::stringstream ss;
            ss << "{\n";
            if (animateNode->target) ss << "  target: document.querySelector('" << *animateNode->target << "'),\n";
            if (animateNode->duration) ss << "  duration: " << *animateNode->duration << ",\n";
            if (animateNode->easing) ss << "  easing: '" << *animateNode->easing << "',\n";
            if (animateNode->loop) ss << "  loop: " << *animateNode->loop << ",\n";
            if (animateNode->direction) ss << "  direction: '" << *animateNode->direction << "',\n";
            if (animateNode->delay) ss << "  delay: " << *animateNode->delay << ",\n";
            if (animateNode->callback) ss << "  callback: " << *animateNode->callback << ",\n";

            if (!animateNode->begin_styles.empty()) {
                ss << "  begin: {\n";
                for (const auto& pair : animateNode->begin_styles) {
                    ss << "    '" << pair.first << "': '" << pair.second << "',\n";
                }
                ss << "  },\n";
            }
            if (!animateNode->when_keyframes.empty()) {
                ss << "  when: [\n";
                for (const auto& keyframe : animateNode->when_keyframes) {
                    ss << "    {\n";
                    ss << "      at: " << keyframe.at << ",\n";
                    for (const auto& pair : keyframe.styles) {
                        ss << "      '" << pair.first << "': '" << pair.second << "',\n";
                    }
                    ss << "    },\n";
                }
                ss << "  ],\n";
            }
            if (!animateNode->end_styles.empty()) {
                ss << "  end: {\n";
                for (const auto& pair : animateNode->end_styles) {
                    ss << "    '" << pair.first << "': '" << pair.second << "',\n";
                }
                ss << "  },\n";
            }
            ss << "}";
            return ss.str();
        }
        case CHTLJSNodeType::Listen:
        case CHTLJSNodeType::EventBinding:
        case CHTLJSNodeType::Delegate:
        case CHTLJSNodeType::VirtualObject:
            return "";
        default:
            return "";
    }
}