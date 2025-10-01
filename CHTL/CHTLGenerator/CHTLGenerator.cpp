#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include <sstream>
#include <iomanip>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {}

std::string CHTLGenerator::generate(const BaseNode* root) {
    if (!root) {
        return "";
    }
    return generateNode(root);
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
    // Note: StyleNode is handled within generateElementNode
    return "";
}

std::string CHTLGenerator::generateElementNode(const ElementNode* node) {
    std::stringstream ss;
    ss << "<" << node->getTagName();

    for (const auto& attr : node->getAttributes()) {
        ss << " " << attr.first << "=\"" << attr.second << "\"";
    }

    if (node->getStyle() && !node->getStyle()->getProperties().empty()) {
        ss << " style=\"";
        for (const auto& prop : node->getStyle()->getProperties()) {
            EvaluatedValue val = m_evaluator.evaluate(prop->getValue());
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

    ss << ">";

    for (const auto& child : node->getChildren()) {
        ss << generateNode(child.get());
    }

    ss << "</" << node->getTagName() << ">";
    return ss.str();
}

std::string CHTLGenerator::generateStyleNode(const StyleNode* node) {
    // This will eventually generate the global style sheet from selector blocks.
    // For now, it does nothing, as inline styles are handled in generateElementNode.
    return "";
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