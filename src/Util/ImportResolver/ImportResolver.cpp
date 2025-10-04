#include "ImportResolver.h"
#include "../../CHTL/CHTLNode/ImportNode.h"
#include "../../CHTL/CHTLNode/OriginNode.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

// Private helper function to recursively resolve imports
void resolveImports(BaseNode& node) {
    auto& children = node.getChildren();

    std::vector<std::unique_ptr<BaseNode>> newChildren;
    bool needsReplacement = false;

    for (auto& child : children) {
        if (child->getType() == NodeType::Import) {
            needsReplacement = true;
            ImportNode* importNode = static_cast<ImportNode*>(child.get());

            const std::string& path = importNode->getPath();
            const std::string& importType = importNode->getImportType();
            const std::string& alias = importNode->getAlias();

            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open imported file: " + path);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            if (importType == "Chtl") {
                CHTLParser parser(content);
                parser.parse();

                auto importedRoot = parser.releaseRoot();
                auto& importedChildren = importedRoot->getChildren();

                for (auto& importedChild : importedChildren) {
                    newChildren.push_back(std::move(importedChild));
                }
            } else {
                // For other types like Html, Css, etc., create an OriginNode
                newChildren.push_back(std::make_unique<OriginNode>(importType, alias, content));
            }
        } else {
            // Recurse on non-import nodes
            resolveImports(*child);
            newChildren.push_back(std::move(child));
        }
    }

    if (needsReplacement) {
        children.clear();
        children = std::move(newChildren);
    }
}

void ImportResolver::resolve(BaseNode& root) {
    resolveImports(root);
}