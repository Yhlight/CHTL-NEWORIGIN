#include "ImportProcessor.h"
#include "ModuleResolver.h"
#include "Loader.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLLexer/Lexer.h"
#include <stdexcept>
#include <filesystem>

ImportProcessor::ImportProcessor(Parser& parser) : parser(parser) {}

void ImportProcessor::process(std::vector<std::unique_ptr<BaseNode>>& nodes) {
    // We iterate backwards because we might be removing nodes
    for (int i = static_cast<int>(nodes.size()) - 1; i >= 0; --i) {
        if (nodes[i]->getType() == NodeType::Import) {
            auto* importNode = static_cast<ImportNode*>(nodes[i].get());
            handleImportNode(*importNode);
            // After processing, the import node itself is no longer needed in the main AST.
            nodes.erase(nodes.begin() + i);
        }
    }
}

void ImportProcessor::handleImportNode(ImportNode& node) {
    switch (node.category) {
        case ImportCategory::Chtl:
        case ImportCategory::Template:
        case ImportCategory::Custom:
        case ImportCategory::Origin: // CHTL Origin definitions
            handleChtlImport(node);
            break;
        case ImportCategory::Html:
        case ImportCategory::Style:
        case ImportCategory::JavaScript:
            handleNonChtlImport(node);
            break;
        default:
            throw std::runtime_error("Unsupported import category.");
    }
}

void ImportProcessor::handleChtlImport(ImportNode& node) {
    ModuleResolver resolver;
    std::filesystem::path current_file_path(parser.sourcePath);
    std::filesystem::path base_path = !parser.sourcePath.empty() && current_file_path.has_parent_path()
                                          ? current_file_path.parent_path()
                                          : std::filesystem::current_path();

    std::vector<std::filesystem::path> resolved_paths = resolver.resolve(node, base_path);
    if (resolved_paths.empty()) {
        throw std::runtime_error("Failed to resolve CHTL import: " + node.path);
    }

    for (const auto& resolved_path : resolved_paths) {
        std::string fileContent = Loader::loadFile(resolved_path.string());
        Lexer importLexer(fileContent);
        Parser importParser(importLexer, resolved_path.string());
        importParser.parse();

        // The logic for merging different import types (precise, wildcard, etc.)
        // will be implemented here. For now, we just merge everything.
        parser.templateManager.merge(importParser.templateManager, {});
    }
}

void ImportProcessor::handleNonChtlImport(ImportNode& node) {
    if (node.alias.empty()) {
        throw std::runtime_error("Import for non-CHTL types must use 'as'.");
    }

    ModuleResolver resolver;
    std::filesystem::path current_file_path(parser.sourcePath);
    std::filesystem::path base_path = !parser.sourcePath.empty() && current_file_path.has_parent_path()
                                          ? current_file_path.parent_path()
                                          : std::filesystem::current_path();

    std::vector<std::filesystem::path> resolved_paths = resolver.resolve(node, base_path);
    if (resolved_paths.empty()) {
        throw std::runtime_error("Failed to resolve non-CHTL import: " + node.path);
    }

    std::string fileContent = Loader::loadFile(resolved_paths[0].string());

    std::string originType;
    switch(node.category) {
        case ImportCategory::Html: originType = "Html"; break;
        case ImportCategory::Style: originType = "Style"; break;
        case ImportCategory::JavaScript: originType = "JavaScript"; break;
        default: break; // Should not happen
    }

    auto originNode = std::make_unique<OriginNode>(originType, fileContent, node.alias);
    parser.templateManager.addNamedOrigin(parser.getCurrentNamespace(), node.alias, std::move(originNode));
}