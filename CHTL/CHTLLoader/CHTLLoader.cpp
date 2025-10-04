#include "CHTLLoader.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h"
#include <fstream>
#include <stdexcept>

namespace CHTL {

CHTLLoader::CHTLLoader(const std::string& basePath) : basePath(basePath) {}

void CHTLLoader::loadImports(const std::shared_ptr<BaseNode>& ast) {
    findAndLoad(ast);
}

const std::map<std::string, std::shared_ptr<BaseNode>>& CHTLLoader::getLoadedAsts() const {
    return loadedAsts;
}

void CHTLLoader::findAndLoad(const std::shared_ptr<BaseNode>& node) {
    if (!node) {
        return;
    }

    if (node->getType() == NodeType::NODE_IMPORT) {
        auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
        if (importNode) {
            std::string path = resolvePath(importNode->getPath());

            // For now, we only handle CHTL imports
            if (importNode->getImportType() == ImportType::CHTL) {
                // Avoid circular dependencies and redundant loads
                if (loadedAsts.find(path) == loadedAsts.end()) {
                    std::ifstream file(path);
                    if (!file) {
                        throw std::runtime_error("Could not open file: " + path);
                    }
                    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                    CHTLLexer lexer(content);
                    std::vector<Token> tokens;
                    Token token = lexer.getNextToken();
                    while (token.type != TokenType::TOKEN_EOF) {
                        tokens.push_back(token);
                        token = lexer.getNextToken();
                    }

                    CHTLParser parser(tokens);
                    auto newAst = parser.parse();

                    if (newAst) {
                        loadedAsts[path] = newAst;
                        // Recursively load imports from the new AST
                        loadImports(newAst);
                    }
                }
            }
        }
    }

    for (const auto& child : node->getChildren()) {
        findAndLoad(child);
    }
}

std::string CHTLLoader::resolvePath(const std::string& path) {
    // For now, a simple implementation. This will be expanded later.
    return basePath + "/" + path;
}

}