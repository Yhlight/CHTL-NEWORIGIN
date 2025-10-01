#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/SelectorBlockNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : m_template_manager(TemplateManager::getInstance()) {}

std::string CHTLGenerator::generate(const BaseNode* root) {
    if (!root) {
        if (!m_global_styles.str().empty()) {
            std::stringstream final_html;
            final_html << "<html><head><style>"
                       << m_global_styles.str()
                       << "</style></head><body>"
                       << ""
                       << "</body></html>";
            return final_html.str();
        }
        return "";
    }
    m_root = root;
    std::string body_content = generateNode(root);

    if (!m_global_styles.str().empty()) {
        std::stringstream final_html;
        final_html << "<html><head><style>"
                   << m_global_styles.str()
                   << "</style></head><body>"
                   << body_content
                   << "</body></html>";
        return final_html.str();
    }

    return body_content;
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
                    EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, node->getStyle());
                    style_ss << prop->getKey() << ": ";
                    if (val.type == EvaluatedValue::Type::NUMBER) {
                        style_ss << val.number_value << val.unit;
                    } else {
                        style_ss << val.string_value;
                    }
                    style_ss << ";";
                } else if (const auto* block = dynamic_cast<const SelectorBlockNode*>(child.get())) {
                    m_global_styles << block->getSelector() << "{";
                    for (const auto& prop : block->getProperties()) {
                        m_global_styles << prop->getKey() << ":";
                        EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, node->getStyle());
                        if (val.type == EvaluatedValue::Type::NUMBER) {
                            std::stringstream temp_ss;
                            temp_ss << val.number_value;
                            m_global_styles << temp_ss.str() << val.unit;
                        } else {
                            m_global_styles << val.string_value;
                        }
                        m_global_styles << ";";
                    }
                    m_global_styles << "}";
                }
            }
        }
        // 2. Add inline properties (will override template properties)
        for (const auto& prop : node->getStyle()->getProperties()) {
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, node->getStyle());
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

    ss << "</" << node->getTagName() << ">";
    return ss.str();
}

void CHTLGenerator::generateStyleNode(const StyleNode* node) {
    for (const auto& block : node->getSelectorBlocks()) {
        m_global_styles << block->getSelector() << "{";
        for (const auto& prop : block->getProperties()) {
            m_global_styles << prop->getKey() << ":";
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, node);
            if (val.type == EvaluatedValue::Type::NUMBER) {
                std::stringstream temp_ss;
                temp_ss << val.number_value;
                m_global_styles << temp_ss.str() << val.unit;
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
    std::stringstream ss;
    ss << "<script>" << node->getContent() << "</script>";
    return ss.str();
}

void CHTLGenerator::generateTemplateDefinition(const TemplateDefinitionNode* node) {
    // This is handled by the parser.
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