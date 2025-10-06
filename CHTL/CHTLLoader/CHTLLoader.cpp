#include "CHTLLoader.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLContext/GenerationContext.h"
#include "../CHTLContext/ConfigurationManager.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <functional> // For std::function

namespace CHTL {

CHTLLoader::CHTLLoader(const std::string& basePath, std::shared_ptr<ConfigurationManager> configManager)
    : basePath(basePath), configManager(configManager) {
    // A better approach would be to get this from an environment variable or config
    this->officialModulePath = "modules";
}

void CHTLLoader::loadImports(const std::shared_ptr<BaseNode>& ast) {
    if (ast) {
        findAndLoad(ast);
    }
}

void CHTLLoader::findAndLoad(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    std::vector<std::shared_ptr<BaseNode>> new_children;
    bool children_modified = false;

    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_IMPORT) {
            children_modified = true; // Mark that we are processing an import
            auto importNode = std::dynamic_pointer_cast<ImportNode>(child);
            if (!importNode) continue;

            std::string path = resolvePath(importNode->getPath());
            ImportType type = importNode->getImportType();

            // Handle CHTL file imports (both full and precise)
            if (type == ImportType::CHTL) {
                // Load and parse the AST if it's not already in our cache
                if (loadedAsts.find(path) == loadedAsts.end()) {
                    std::ifstream file(path);
                    if (!file) throw std::runtime_error("Could not open file: " + path);
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                    CHTLLexer lexer(content, configManager);
                    std::vector<Token> tokens;
                    Token token = lexer.getNextToken();
                    while (token.type != TokenType::TOKEN_EOF) {
                        tokens.push_back(token);
                        token = lexer.getNextToken();
                    }
                    CHTLParser parser(tokens, configManager);
                    loadedAsts[path] = parser.parse();
                    // Recursively process imports within the newly loaded file
                    findAndLoad(loadedAsts[path]);
                }
                auto importedAst = loadedAsts[path];

                // Case 1: Full import of a CHTL file (e.g., [Import] @Chtl from "...")
                if (importNode->getCategory() == ImportCategory::NONE && importNode->getItemType() == "@Chtl") {
                    std::string namespaceName;
                    bool explicitNamespaceFound = false;

                    // Check for an explicit [Namespace] block in the imported file
                    for (const auto& importedChild : importedAst->getChildren()) {
                        if (importedChild->getType() == NodeType::NODE_NAMESPACE) {
                            auto nsNode = std::dynamic_pointer_cast<NamespaceNode>(importedChild);
                            namespaceName = nsNode->getName();
                            // We use the existing namespace node directly
                            new_children.push_back(importedChild);
                            explicitNamespaceFound = true;
                            break;
                        }
                    }

                    // If no explicit namespace, create one from the filename
                    if (!explicitNamespaceFound) {
                         namespaceName = std::filesystem::path(path).stem().string();
                         auto newNsNode = std::make_shared<NamespaceNode>(namespaceName);
                         newNsNode->setChildren(importedAst->getChildren());
                         new_children.push_back(newNsNode);
                    }
                }
                // Case 2: Precise import (e.g., [Import] [Template] @Style MyStyle from "...")
                else {
                    auto foundNodes = findNodes(importedAst, importNode->getCategory(), importNode->getItemType(), importNode->getItemName());
                    for (const auto& foundNode : foundNodes) {
                        std::string key = importNode->getAlias();
                        if (key.empty()) {
                            if (auto custom = std::dynamic_pointer_cast<CustomNode>(foundNode)) key = custom->getName();
                            else if (auto templ = std::dynamic_pointer_cast<TemplateNode>(foundNode)) key = templ->getName();
                        }
                        if (!key.empty()) {
                            importedItems[key] = foundNode;
                        }
                    }
                }
            }
            // Handle non-CHTL imports (HTML, CSS, JS)
            else {
                std::string alias = importNode->getAlias();
                if (namedOriginNodes.find(alias) == namedOriginNodes.end()) {
                    std::ifstream file(path);
                    if (!file) throw std::runtime_error("Could not open file for raw import: " + path);
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                    std::string originTypeStr = (type == ImportType::HTML) ? "@Html" : (type == ImportType::STYLE) ? "@Style" : "@JavaScript";
                    namedOriginNodes[alias] = std::make_shared<OriginNode>(originTypeStr, alias, content);
                }
            }
        } else {
            // If it's not an import node, process its children recursively first
            findAndLoad(child);
            new_children.push_back(child);
        }
    }

    // If we processed any imports, we need to update the parent's children list
    if (children_modified) {
        node->setChildren(new_children);
    }
}

// Helper functions to convert string representations to enum types
CustomType stringToCustomType(const std::string& typeStr) {
    if (typeStr == "@Element") return CustomType::ELEMENT;
    if (typeStr == "@Style") return CustomType::STYLE;
    if (typeStr == "@Var") return CustomType::VAR;
    throw std::runtime_error("Unknown custom type string: " + typeStr);
}

TemplateType stringToTemplateType(const std::string& typeStr) {
    if (typeStr == "@Element") return TemplateType::ELEMENT;
    if (typeStr == "@Style") return TemplateType::STYLE;
    if (typeStr == "@Var") return TemplateType::VAR;
    throw std::runtime_error("Unknown template type string: " + typeStr);
}

std::vector<std::shared_ptr<BaseNode>> CHTLLoader::findNodes(const std::shared_ptr<BaseNode>& ast, ImportCategory category, const std::string& itemType, const std::string& itemName) {
    std::vector<std::shared_ptr<BaseNode>> found;
    if (!ast) return found;

    std::function<void(const std::shared_ptr<BaseNode>&)> search =
        [&](const std::shared_ptr<BaseNode>& node) {
        if (!node) return;

        bool match = false;
        if (node->getType() == NodeType::NODE_CUSTOM && category == ImportCategory::CUSTOM) {
            auto custom = std::dynamic_pointer_cast<CustomNode>(node);
            if (custom->getCustomType() == stringToCustomType(itemType) && (itemName.empty() || itemName == custom->getName())) {
                match = true;
            }
        } else if (node->getType() == NodeType::NODE_TEMPLATE && category == ImportCategory::TEMPLATE) {
            auto templ = std::dynamic_pointer_cast<TemplateNode>(node);
            if (templ->getTemplateType() == stringToTemplateType(itemType) && (itemName.empty() || itemName == templ->getName())) {
                match = true;
            }
        }

        if (match) {
            found.push_back(node);
            if (!itemName.empty()) return;
        }

        for (const auto& child : node->getChildren()) {
            search(child);
        }
    };

    search(ast);
    return found;
}

std::string CHTLLoader::resolvePath(const std::string& importPath) {
    namespace fs = std::filesystem;
    fs::path path(importPath);

    // If it's an absolute path or contains directory separators, treat it as a relative path
    if (path.is_absolute() || importPath.find('/') != std::string::npos || importPath.find('\\') != std::string::npos) {
        fs::path fullPath = fs::path(basePath) / path;
        if (fs::exists(fullPath)) {
            return fs::weakly_canonical(fullPath).string();
        }
    }

    // Otherwise, search in predefined locations
    std::vector<fs::path> searchPaths;
    searchPaths.push_back(fs::path(basePath)); // 1. Current directory
    fs::path localModulePath = fs::path(basePath) / "module";
    if (fs::exists(localModulePath)) searchPaths.push_back(localModulePath); // 2. Local module folder
    if (fs::exists(officialModulePath)) searchPaths.push_back(fs::path(officialModulePath)); // 3. Official module folder

    const std::vector<std::string> extensions = {".chtl", ".cmod", ".html", ".css", ".js"};
    for (const auto& dir : searchPaths) {
        // Search for file with extension
        if (path.has_extension()) {
            fs::path potentialPath = dir / path;
            if (fs::exists(potentialPath)) return fs::weakly_canonical(potentialPath).string();
        }
        // Search for file by trying different extensions
        else {
            for (const auto& ext : extensions) {
                fs::path potentialPath = dir / (importPath + ext);
                if (fs::exists(potentialPath)) return fs::weakly_canonical(potentialPath).string();
            }
        }
    }

    throw std::runtime_error("Could not resolve import path: " + importPath);
}

// gatherTemplates now needs to be namespace-aware
void CHTLLoader::gatherTemplates(const std::shared_ptr<BaseNode>& ast, GenerationContext& context) {
    // Overload to start the process without a namespace
    std::function<void(const std::shared_ptr<BaseNode>&, const std::string&)> processNode =
        [&](const std::shared_ptr<BaseNode>& node, const std::string& current_namespace) {
        if (!node) return;

        std::string next_namespace = current_namespace;

        if (node->getType() == NodeType::NODE_NAMESPACE) {
            auto nsNode = std::dynamic_pointer_cast<NamespaceNode>(node);
            next_namespace = nsNode->getName();
        } else if (node->getType() == NodeType::NODE_TEMPLATE) {
            auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node);
            if (templateNode) {
                context.addTemplate(templateNode->getName(), templateNode, current_namespace);
            }
        }

        for (const auto& child : node->getChildren()) {
            processNode(child, next_namespace);
        }
    };

    processNode(ast, GLOBAL_NAMESPACE);
}

const std::map<std::string, std::shared_ptr<BaseNode>>& CHTLLoader::getLoadedAsts() const {
    return loadedAsts;
}

const std::map<std::string, std::shared_ptr<OriginNode>>& CHTLLoader::getNamedOriginNodes() const {
    return namedOriginNodes;
}

const std::map<std::string, std::shared_ptr<BaseNode>>& CHTLLoader::getImportedItems() const {
    return importedItems;
}

const std::map<std::string, std::map<std::string, std::shared_ptr<BaseNode>>>& CHTLLoader::getNamespaces() const {
    // This method is now somewhat obsolete as namespaces are directly in the AST.
    // It could be kept for debugging or removed.
    return namespaces;
}

}