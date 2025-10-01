#include "CHTLGenerator.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/SelectorBlockNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/CustomDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/DeleteNode.h"
#include "CHTLNode/InsertNode.h"
#include "CHTLNode/OriginNode.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <map>
#include <set>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : m_template_manager(TemplateManager::getInstance()) {}

std::string CHTLGenerator::generate(const DocumentNode* root) {
    if (!root) {
        return "";
    }

    m_root = root;
    std::stringstream body_content;
    for (const auto& child : root->children) {
        body_content << generateNode(child.get());
    }

    std::stringstream final_html;
    if (root->useHtml5) {
        final_html << "<!DOCTYPE html>\\n";
    }

    if (!m_global_styles.str().empty()) {
        final_html << "<html><head><style>"
                   << m_global_styles.str()
                   << "</style></head><body>"
                   << body_content.str()
                   << "</body></html>";
    } else {
        final_html << body_content.str();
    }

    return final_html.str();
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
    if (const auto* originNode = dynamic_cast<const OriginNode*>(node)) {
        return generateOriginNode(originNode);
    }
    if (dynamic_cast<const TemplateDefinitionNode*>(node) || dynamic_cast<const CustomDefinitionNode*>(node)) {
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
        ss << " " << attr.first << "=\\\"" << attr.second << "\\\"";
    }

    if (node->getStyle()) {
        std::stringstream style_ss;
        for (const auto& usage : node->getStyle()->getTemplateUsages()) {
            expandStyleTemplate(style_ss, usage.get());
        }
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
            ss << " style=\\\"" << style_ss.str() << "\\\"";
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

void CHTLGenerator::collectProperties(const BaseNode* templateDef, std::map<std::string, std::string>& properties) {
    const std::vector<std::unique_ptr<BaseNode>>* children = nullptr;

    if (const auto* def = dynamic_cast<const TemplateDefinitionNode*>(templateDef)) {
        children = &def->getChildren();
    } else if (const auto* customDef = dynamic_cast<const CustomDefinitionNode*>(templateDef)) {
        children = &customDef->getChildren();
    }

    if (!children) return;

    for (const auto& child : *children) {
        if (const auto* prop = dynamic_cast<const StylePropertyNode*>(child.get())) {
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, nullptr);
            std::stringstream val_ss;
            if (val.type == EvaluatedValue::Type::NUMBER) {
                val_ss << val.number_value << val.unit;
            } else {
                val_ss << val.string_value;
            }
            properties[prop->getKey()] = val_ss.str();
        } else if (const auto* usage = dynamic_cast<const TemplateUsageNode*>(child.get())) {
            const auto* nestedDef = m_template_manager.getTemplate(usage->getName());
            if (!nestedDef) {
                throw std::runtime_error("Inherited template not found: " + usage->getName());
            }
            collectProperties(nestedDef, properties);
        }
    }
}

void CHTLGenerator::expandStyleTemplate(std::stringstream& ss, const TemplateUsageNode* usage) {
    const auto* def = m_template_manager.getTemplate(usage->getName());
    if (!def) {
        throw std::runtime_error("Style template not found: " + usage->getName());
    }

    std::map<std::string, std::string> properties;
    collectProperties(def, properties);

    std::set<std::string> deleted_properties;
    for (const auto& spec : usage->getSpecializations()) {
        if (const auto* deleteNode = dynamic_cast<const DeleteNode*>(spec.get())) {
            for (const auto& target : deleteNode->getTargets()) {
                deleted_properties.insert(target);
            }
        }
    }

    for(auto const& [key, val] : properties) {
        if (deleted_properties.find(key) == deleted_properties.end()) {
            ss << key << ": " << val << ";";
        }
    }
}


void CHTLGenerator::generateStyleNode(const StyleNode* node) {
    for (const auto& block : node->getSelectorBlocks()) {
        m_global_styles << block->getSelector() << " {";
        for (const auto& prop : block->getProperties()) {
            m_global_styles << prop->getKey() << ": ";
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, node);
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

std::string CHTLGenerator::generateOriginNode(const OriginNode* node) {
    switch (node->getOriginType()) {
        case OriginType::HTML:
            return node->getContent();
        case OriginType::STYLE:
            m_global_styles << node->getContent();
            return "";
        case OriginType::JAVASCRIPT:
            return "<script>" + node->getContent() + "</script>";
    }
    return "";
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

    if (const auto* elementDef = dynamic_cast<const TemplateDefinitionNode*>(def)) {
        if (elementDef->getTemplateType() == TemplateType::ELEMENT) {
            std::stringstream ss;
            for (const auto& child : elementDef->getChildren()) {
                ss << generateNode(child.get());
            }
            return ss.str();
        }
    } else if (const auto* customElementDef = dynamic_cast<const CustomDefinitionNode*>(def)) {
        if (customElementDef->getTemplateType() == TemplateType::ELEMENT) {
            // This is where insert/delete logic for elements would go.
            // For now, just generate the base children.
            std::stringstream ss;
            for (const auto& child : customElementDef->getChildren()) {
                ss << generateNode(child.get());
            }
            return ss.str();
        }
    }

    return "";
}

} // namespace CHTL