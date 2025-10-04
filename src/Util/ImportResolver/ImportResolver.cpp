#include "ImportResolver.h"
#include "../../CHTL/CHTLNode/ImportNode.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>

// Private helper function to recursively resolve imports
void resolveImports(BaseNode& node) {
    auto& children = node.getChildren();

    // Using a temporary vector to store new children to avoid iterator invalidation
    std::vector<std::unique_ptr<BaseNode>> newChildren;
    bool needsReplacement = false;

    for (auto& child : children) {
        if (child->getType() == NodeType::Import) {
            needsReplacement = true;
            ImportNode* importNode = static_cast<ImportNode*>(child.get());
            const std::string& path = importNode->getPath();

            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open imported file: " + path);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            CHTLParser parser(content);
            parser.parse();

            auto importedRoot = parser.releaseRoot();
            auto& importedChildren = importedRoot->getChildren();

            for (auto& importedChild : importedChildren) {
                newChildren.push_back(std::move(importedChild));
            }
        } else {
            // Recurse on non-import nodes
            resolveImports(*child);
            newChildren.push_back(std::move(child));
        }
    }

    if (needsReplacement) {
        children.clear();
        for(auto& newChild : newChildren) {
            children.push_back(std::move(newChild));
        }
    }
}

void ImportResolver::resolve(BaseNode& root) {
    resolveImports(root);
}