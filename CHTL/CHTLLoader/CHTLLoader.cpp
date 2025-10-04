#include "CHTLLoader.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace CHTL {

CHTLLoader::CHTLLoader(const std::string& basePath) : basePath(basePath) {
    this->officialModulePath = "modules";
}

void CHTLLoader::loadImports(const std::shared_ptr<BaseNode>& ast) {
    findAndLoad(ast);
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
    return namespaces;
}

void CHTLLoader::findAndLoad(const std::shared_ptr<BaseNode>& node) {
    if (!node) {
        return;
    }

    if (node->getType() == NodeType::NODE_IMPORT) {
        auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
        if (importNode) {
            std::string path = resolvePath(importNode->getPath());

            if (importNode->getImportType() == ImportType::CHTL) {
                if (loadedAsts.find(path) == loadedAsts.end()) {
                     std::ifstream file(path);
                    if (!file) throw std::runtime_error("Could not open file: " + path);
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    CHTLLexer lexer(content);
                    std::vector<Token> tokens;
                    Token token = lexer.getNextToken();
                    while (token.type != TokenType::TOKEN_EOF) {
                        tokens.push_back(token);
                        token = lexer.getNextToken();
                    }
                    CHTLParser parser(tokens);
                    loadedAsts[path] = parser.parse();
                }

                auto importedAst = loadedAsts[path];

                // Handle whole-file CHTL import with namespace
                if (importNode->getCategory() == ImportCategory::NONE && importNode->getItemType() == "@Chtl") {
                    std::string namespaceName = "";
                    bool namespaceFound = false;
                    for (const auto& child : importedAst->getChildren()) {
                        if (child->getType() == NodeType::NODE_NAMESPACE) {
                            auto nsNode = std::dynamic_pointer_cast<NamespaceNode>(child);
                            namespaceName = nsNode->getName();
                            namespaceFound = true;
                            for (const auto& nsChild : nsNode->getChildren()) {
                                if (auto custom = std::dynamic_pointer_cast<CustomNode>(nsChild)) namespaces[namespaceName][custom->getName()] = nsChild;
                                else if (auto templ = std::dynamic_pointer_cast<TemplateNode>(nsChild)) namespaces[namespaceName][templ->getName()] = nsChild;
                            }
                            break;
                        }
                    }
                    if (!namespaceFound) {
                        namespaceName = std::filesystem::path(path).stem().string();
                        for (const auto& child : importedAst->getChildren()) {
                            if (auto custom = std::dynamic_pointer_cast<CustomNode>(child)) namespaces[namespaceName][custom->getName()] = child;
                            else if (auto templ = std::dynamic_pointer_cast<TemplateNode>(child)) namespaces[namespaceName][templ->getName()] = child;
                        }
                    }
                }
                // Handle precise and type-based imports
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
            else {
                 std::string alias = importNode->getAlias();
                 if (namedOriginNodes.find(alias) == namedOriginNodes.end()) {
                    std::ifstream file(path);
                    if (!file) throw std::runtime_error("Could not open file: " + path);
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    if (importNode->getImportType() == ImportType::HTML) {
                        namedOriginNodes[alias] = std::make_shared<OriginNode>("@Html", alias, content);
                    } else if (importNode->getImportType() == ImportType::STYLE) {
                        namedOriginNodes[alias] = std::make_shared<OriginNode>("@Style", alias, content);
                    } else if (importNode->getImportType() == ImportType::JAVASCRIPT) {
                        namedOriginNodes[alias] = std::make_shared<OriginNode>("@JavaScript", alias, content);
                    }
                 }
            }
        }
    }

    for (const auto& child : node->getChildren()) {
        findAndLoad(child);
    }
}

std::vector<std::shared_ptr<BaseNode>> CHTLLoader::findNodes(const std::shared_ptr<BaseNode>& ast, ImportCategory category, const std::string& itemType, const std::string& itemName) {
    std::vector<std::shared_ptr<BaseNode>> found;
    if (!ast) return found;

    for (const auto& child : ast->getChildren()) {
        bool match = false;
        if (child->getType() == NodeType::NODE_CUSTOM && category == ImportCategory::CUSTOM) {
            auto custom = std::dynamic_pointer_cast<CustomNode>(child);
            bool typeMatch = (itemType == "@Element" && custom->getCustomType() == CustomType::ELEMENT) ||
                             (itemType == "@Style" && custom->getCustomType() == CustomType::STYLE) ||
                             (itemType == "@Var" && custom->getCustomType() == CustomType::VAR);
            if (typeMatch && (itemName.empty() || itemName == custom->getName())) {
                match = true;
            }
        } else if (child->getType() == NodeType::NODE_TEMPLATE && category == ImportCategory::TEMPLATE) {
            auto templ = std::dynamic_pointer_cast<TemplateNode>(child);
            bool typeMatch = (itemType == "@Element" && templ->getTemplateType() == TemplateType::ELEMENT) ||
                             (itemType == "@Style" && templ->getTemplateType() == TemplateType::STYLE) ||
                             (itemType == "@Var" && templ->getTemplateType() == TemplateType::VAR);
            if (typeMatch && (itemName.empty() || itemName == templ->getName())) {
                match = true;
            }
        }

        if (match) {
            found.push_back(child);
        }

        auto deeperNodes = findNodes(child, category, itemType, itemName);
        found.insert(found.end(), deeperNodes.begin(), deeperNodes.end());
    }

    return found;
}


std::string CHTLLoader::resolvePath(const std::string& importPath) {
    namespace fs = std::filesystem;
    fs::path path(importPath);

    if (importPath.find('/') != std::string::npos || importPath.find('\\') != std::string::npos) {
        fs::path fullPath = fs::path(basePath) / path;
        if (fs::exists(fullPath) && fs::is_regular_file(fullPath)) {
            return fs::weakly_canonical(fullPath).string();
        }
        if (fs::exists(fullPath) && fs::is_directory(fullPath)) {
            throw std::runtime_error("Import path cannot be a directory: " + importPath);
        }
        throw std::runtime_error("Could not find file for specific path: " + importPath);
    }

    std::vector<fs::path> searchPaths;
    if (fs::exists(officialModulePath) && fs::is_directory(officialModulePath)) {
        searchPaths.push_back(fs::path(officialModulePath));
    }
    fs::path localModulePath = fs::path(basePath) / "module";
    if (fs::exists(localModulePath) && fs::is_directory(localModulePath)) {
        searchPaths.push_back(localModulePath);
    }
    searchPaths.push_back(fs::path(basePath));

    if (path.has_extension()) {
        for (const auto& dir : searchPaths) {
            fs::path potentialPath = dir / path;
            if (fs::exists(potentialPath) && fs::is_regular_file(potentialPath)) {
                return fs::weakly_canonical(potentialPath).string();
            }
        }
    }
    else {
        const std::vector<std::string> extensions = {".cmod", ".chtl", ".html", ".css", ".js"};
        for (const auto& dir : searchPaths) {
            for (const auto& ext : extensions) {
                fs::path potentialPath = dir / (importPath + ext);
                if (fs::exists(potentialPath) && fs::is_regular_file(potentialPath)) {
                    return fs::weakly_canonical(potentialPath).string();
                }
            }
        }
    }

    throw std::runtime_error("Could not resolve import path: " + importPath);
}

}