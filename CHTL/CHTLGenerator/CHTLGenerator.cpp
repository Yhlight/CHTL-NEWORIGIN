#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/SelectorBlockNode.h"
#include <sstream>
#include <iomanip>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {}

std::string CHTLGenerator::generate(const BaseNode* root) {
    if (!root) {
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
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(node)) {
        return generateElementNode(elementNode);
    }
    if (const auto* textNode = dynamic_cast<const TextNode*>(node)) {
        return generateTextNode(textNode);
    }
     if (const auto* scriptNode = dynamic_cast<const ScriptNode*>(node)) {
        return generateScriptNode(scriptNode);
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
        if (!node->getStyle()->getProperties().empty()) {
            ss << " style=\"";
            for (const auto& prop : node->getStyle()->getProperties()) {
                EvaluatedValue val = m_evaluator.evaluate(prop->getValue(), m_root, node->getStyle());
                ss << prop->getKey() << ": ";
                if (val.type == EvaluatedValue::Type::NUMBER) {
                    ss << val.number_value << val.unit;
                } else {
                    ss << val.string_value;
                }
                ss << ";";
            }
            ss << "\"";
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
        m_global_styles << block->getSelector() << " {";
        for (const auto& prop : block->getProperties()) {
            m_global_styles << prop->getKey() << ":";
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

std::string CHTLGenerator::generateScriptNode(const ScriptNode* node) {
    std::stringstream ss;
    ss << "<script>" << node->getContent() << "</script>";
    return ss.str();
}

} // namespace CHTL