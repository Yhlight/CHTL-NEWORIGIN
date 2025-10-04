#include "ImportResolver.h"
#include "../../CHTL/CHTLNode/ImportNode.h"
#include "../../CHTL/CHTLNode/OriginNode.h"
#include "../../CHTL/CHTLNode/TemplateDefinitionNode.h"
#include "../../CHTL/CHTLNode/NamespaceNode.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <filesystem>

// Helper function to find a specific definition in an AST
static BaseNode* findDefinition(BaseNode* root, const std::string& category, const std::string& type, const std::string& name) {
    if (!root) {
        return nullptr;
    }

    if (root->getType() == NodeType::TemplateDefinition) {
        TemplateDefinitionNode* defNode = static_cast<TemplateDefinitionNode*>(root);
        if (defNode->getCategory() == category && defNode->getTemplateType() == type && defNode->getTemplateName() == name) {
            return root;
        }
    }

    for (const auto& child : root->getChildren()) {
        BaseNode* found = findDefinition(child.get(), category, type, name);
        if (found) {
            return found;
        }
    }

    return nullptr;
}


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
            const std::string& category = importNode->getCategory();
            const std::string& importType = importNode->getImportType();
            const std::string& name = importNode->getName();
            const std::string& alias = importNode->getAlias();

            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open imported file: " + path);
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            if (!name.empty()) { // Precise import
                CHTLParser parser(content);
                parser.parse();
                auto importedRoot = parser.releaseRoot();

                BaseNode* definition = findDefinition(importedRoot.get(), category, importType, name);

                if (definition) {
                    newChildren.push_back(definition->clone());
                } else {
                    throw std::runtime_error("Could not find definition for " + category + " @" + importType + " " + name + " in " + path);
                }
            } else if (!importType.empty() && importType == "Chtl") { // Full Chtl file import
                CHTLParser parser(content);
                parser.parse();
                auto importedRoot = parser.releaseRoot();
                auto& importedChildren = importedRoot->getChildren();

                // Check if the file already defines a namespace
                bool hasNamespace = false;
                for(const auto& importedChild : importedChildren) {
                    if (importedChild->getType() == NodeType::Namespace) {
                        hasNamespace = true;
                        break;
                    }
                }

                if (hasNamespace) {
                     for (auto& importedChild : importedChildren) {
                        newChildren.push_back(std::move(importedChild));
                    }
                } else {
                    // Create a default namespace using the filename
                    std::string filename = std::filesystem::path(path).stem().string();
                    auto namespaceNode = std::make_unique<NamespaceNode>(filename);
                    for (auto& importedChild : importedChildren) {
                        namespaceNode->addChild(std::move(importedChild));
                    }
                    newChildren.push_back(std::move(namespaceNode));
                }

            } else { // Raw content import
                newChildren.push_back(std::make_unique<OriginNode>(importType, content, alias));
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