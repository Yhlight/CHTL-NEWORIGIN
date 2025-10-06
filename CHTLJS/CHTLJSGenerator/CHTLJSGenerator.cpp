#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include <stdexcept>

namespace CHTLJS {

std::string CHTLJSGenerator::generate(const std::shared_ptr<BaseNode>& node) {
    visit(node);
    return out.str();
}

void CHTLJSGenerator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::NODE_SCRIPT_LOADER: {
            auto scriptLoaderNode = std::dynamic_pointer_cast<ScriptLoaderNode>(node);
            for (const auto& script : scriptLoaderNode->scripts) {
                out << "require(\"" << script << "\");\n";
            }
            break;
        }
        default:
            throw std::runtime_error("Unknown CHTLJS node type.");
    }
}

}