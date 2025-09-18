#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJSNode.h"
#include <sstream>

namespace CHTL_JS {

std::string CHTLJSGenerator::generateSubtree(CHTLJS_BaseNode& node) {
    m_output = "";
    node.accept(*this);
    return m_output;
}

std::string CHTLJSGenerator::generate(std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes) {
    std::stringstream ss;
    for (auto& node : nodes) {
        ss << generateSubtree(*node);
    }
    return ss.str();
}

void CHTLJSGenerator::visit(EnhancedSelectorNode& node) {
    // This is a simplified translation. A real implementation would need to handle
    // different selector types (.class, #id, tag) more robustly.
    m_output = "document.querySelector('" + node.selector + "')";
}

void CHTLJSGenerator::visit(RawJSNode& node) {
    m_output = node.js_code;
}

void CHTLJSGenerator::visit(ListenNode& node) {
    std::string target_code = generateSubtree(*node.target);
    std::stringstream ss;

    for (const auto& pair : node.events) {
        const std::string& eventName = pair.first;
        const std::string& functionBody = pair.second;
        ss << target_code << ".addEventListener('" << eventName << "', " << functionBody << ");\n";
    }
    m_output = ss.str();
}


} // namespace CHTL_JS
