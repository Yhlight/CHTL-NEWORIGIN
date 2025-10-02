#include "CHTLProcessor.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include <vector>
#include <queue>

// A helper struct to manage the processing queue
struct FileToProcess {
    std::string path;
    std::string namespaceAlias;
};

// Recursively traverses the AST to find all ImportNode instances.
void findImports(BaseNode* node, std::vector<ImportNode*>& imports) {
    if (!node) return;

    if (auto importNode = dynamic_cast<ImportNode*>(node)) {
        imports.push_back(importNode);
    }

    if (auto docNode = dynamic_cast<DocumentNode*>(node)) {
        for (const auto& child : docNode->getChildren()) {
            findImports(child.get(), imports);
        }
    } else if (auto nsNode = dynamic_cast<NamespaceNode*>(node)) {
        for (const auto& child : nsNode->getChildren()) {
            findImports(child.get(), imports);
        }
    }
}

CHTLProcessor::CHTLProcessor(const std::string& entryFilePath)
    : entryFile(entryFilePath) {}

std::unique_ptr<DocumentNode> CHTLProcessor::process() {
    std::queue<FileToProcess> tasks;
    tasks.push({entryFile, ""}); // The entry file starts in the global namespace.

    // Pass 1: Discovery and Context Population
    while (!tasks.empty()) {
        FileToProcess currentTask = tasks.front();
        tasks.pop();

        if (processedFiles.count(currentTask.path)) {
            continue; // Avoid circular dependencies and redundant processing.
        }
        processedFiles.insert(currentTask.path);

        if (!currentTask.namespaceAlias.empty()) {
            context.enterNamespace(currentTask.namespaceAlias);
        }

        std::string content = CHTLLoader::loadFile(currentTask.path);
        // Parse in discovery mode to find all imports and populate context without resolving cross-file templates.
        CHTLParser parser(content, context, true);
        std::unique_ptr<DocumentNode> tempDoc = parser.parse();

        std::vector<ImportNode*> imports;
        findImports(tempDoc.get(), imports);

        for (const auto* importNode : imports) {
            tasks.push({importNode->getFilePath(), importNode->getAlias()});
        }

        if (!currentTask.namespaceAlias.empty()) {
            context.leaveNamespace();
        }
    }

    // Pass 2: Final Resolution and AST Generation
    std::string finalContent = CHTLLoader::loadFile(entryFile);
    // Parse in normal mode to resolve all templates from the now-populated context.
    CHTLParser finalParser(finalContent, context, false);
    return finalParser.parse();
}

// This private method is not needed for the two-pass queue-based design.
void CHTLProcessor::processFile(const std::string& filePath) {
    // Stub
}