#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"
#include "../CHTLJSNode/RawJSNode.h"
#include <sstream>

namespace CHTL_JS {

std::string CHTLJSGenerator::generate(std::vector<std::unique_ptr<CHTLJS_BaseNode>>& nodes) {
    std::stringstream ss;
    for (auto& node : nodes) {
        m_output = ""; // Clear previous output
        node->accept(*this);
        ss << m_output;
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

} // namespace CHTL_JS
