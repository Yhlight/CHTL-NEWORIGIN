#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/RawScriptNode.h"
#include "CHTLNode/EnhancedSelectorNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/SelectorBlockNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : m_template_manager(TemplateManager::getInstance()) {}

std::string CHTLGenerator::generate(std::vector<std::unique_ptr<BaseNode>>& nodes) {
    std::stringstream body_content_ss;

    // Find the first element node to act as the root for expression evaluation
    for (const auto& node : nodes) {
        if (dynamic_cast<const ElementNode*>(node.get())) {
            m_root_element = node.get();
            break;
        }
    }

    // If no root element is found, just use the first node
    if (!m_root_element && !nodes.empty()) {
        m_root_element = nodes[0].get();
    }

    // Generate content for all root-level nodes
    for (const auto& node : nodes) {
        body_content_ss << generateNode(node.get());
    }

    std::string body_content = body_content_ss.str();

    // If there's any content (body or styles), wrap it in a full HTML document
    if (!body_content.empty() || !m_global_styles.str().empty()) {
        std::stringstream final_html;
        final_html << "<html><head><style>"
                   << m_global_styles.str()
                   << "</style></head><body>"
                   << body_content
                   << "</body></html>";
        return final_html.str();
    }

    return ""; // Return empty string if no content was generated
}

std::string CHTLGenerator::generateNode(const BaseNode* node) {
    if (!node) return "";
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(node)) {
        return generateElementNode(elementNode);
    }
    if (const auto* textNode = dynamic_cast<const TextNode*>(node)) {
        return generateTextNode(textNode);
    }
    if (const auto* scriptNode = dynamic_cast<const ScriptNode*>(node)) {
        return generateScriptNode(scriptNode);
    }
    if (dynamic_cast<const TemplateDefinitionNode*>(node)) {
        // Template definitions are handled by the parser/manager and don't produce output here.
        return "";
    }
    if (const auto* templateUsageNode = dynamic_cast<const TemplateUsageNode*>(node)) {
        return generateTemplateUsage(templateUsageNode);
    }
    return "";
}

std::string CHTLGenerator::generateElementNode(const ElementNode* node) {
    std::stringstream ss;
    ss << "<" << node->getTagName();

    for (const auto& attr : node->getAttributes()) {
        ss << " " << attr.first << "=\"" << attr.second << "\"";
    }

    if (node->getStyle()) {
        std::stringstream style_ss;
        // 1. Expand style templates first
        for (const auto& usage : node->getStyle()->getTemplateUsages()) {
             const auto* def = m_template_manager.getTemplate(usage->getName());
            if (!def || def->getTemplateType() != TemplateType::STYLE) {
                throw std::runtime_error("Style template not found: " + usage->getName());
            }
            for (const auto& child : def->getChildren()) {
                if (const auto* prop = dynamic_cast<const StylePropertyNode*>(child.get())) {
                    EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root_element, node->getStyle());
                     style_ss << prop->getKey() << ": ";
                    if (val.type == EvaluatedValue::Type::NUMBER) {
                        style_ss << val.number_value << val.unit;
                    } else {
                        style_ss << val.string_value;
                    }
                    style_ss << ";";
                }
            }
        }
        // 2. Add inline properties (will override template properties)
        for (const auto& prop : node->getStyle()->getProperties()) {
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root_element, node->getStyle());
            style_ss << prop->getKey() << ": ";
            if (val.type == EvaluatedValue::Type::NUMBER) {
                style_ss << val.number_value << val.unit;
            } else {
                style_ss << val.string_value;
            }
            style_ss << ";";
        }

        if (!style_ss.str().empty()) {
            ss << " style=\"" << style_ss.str() << "\"";
        }

        generateStyleNode(node->getStyle());
    }

    ss << ">";

    for (const auto& child : node->getChildren()) {
        ss << generateNode(child.get());
    }

    if (node->getScript()) {
        ss << generateScriptNode(node->getScript());
    }

    ss << "</" << node->getTagName() << ">";
    return ss.str();
}

void CHTLGenerator::generateStyleNode(const StyleNode* node) {
    for (const auto& block : node->getSelectorBlocks()) {
        m_global_styles << block->getSelector() << " {";
        for (const auto& prop : block->getProperties()) {
            m_global_styles << prop->getKey() << ": ";
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root_element, node);
            if (val.type == EvaluatedValue::Type::NUMBER) {
                m_global_styles << val.number_value << val.unit;
            } else {
                m_global_styles << val.string_value;
            }
            m_global_styles << ";";
        }
        m_global_styles << "}";
    }
}

std::string CHTLGenerator::generateTextNode(const TextNode* node) {
    return node->getValue();
}

std::string CHTLGenerator::generateScriptNode(const ScriptNode* node) {
    std::stringstream script_content_ss;
    for (const auto& child : node->getChildren()) {
        if (const auto* rawNode = dynamic_cast<const RawScriptNode*>(child.get())) {
            script_content_ss << rawNode->getScript();
        } else if (const auto* selectorNode = dynamic_cast<const EnhancedSelectorNode*>(child.get())) {
            std::string selector_str = selectorNode->getSelector();
            size_t bracket_pos = selector_str.find_last_of('[');
            if (bracket_pos != std::string::npos && selector_str.back() == ']') {
                std::string selector = selector_str.substr(0, bracket_pos);
                std::string index = selector_str.substr(bracket_pos);
                script_content_ss << "document.querySelectorAll('" << selector << "')" << index;
            } else {
                script_content_ss << "document.querySelector('" << selector_str << "')";
            }
        }
    }

    std::stringstream ss;
    ss << "<script>" << script_content_ss.str() << "</script>";
    return ss.str();
}

void CHTLGenerator::generateTemplateDefinition(const TemplateDefinitionNode* node) {
    // This is handled by the parser/manager.
}

std::string CHTLGenerator::generateTemplateUsage(const TemplateUsageNode* node) {
    const auto* def = m_template_manager.getTemplate(node->getName());
    if (!def) {
        throw std::runtime_error("Template not found: " + node->getName());
    }

    std::stringstream ss;
    if (def->getTemplateType() == TemplateType::ELEMENT) {
        for (const auto& child : def->getChildren()) {
            ss << generateNode(child.get());
        }
    } else if (def->getTemplateType() == TemplateType::STYLE) {
        // This is handled in generateElementNode via its style node
    }
    return ss.str();
}

} // namespace CHTL