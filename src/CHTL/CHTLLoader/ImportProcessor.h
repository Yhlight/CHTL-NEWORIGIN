#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLParser/Parser.h" // For Parser context
#include <vector>
#include <memory>

class ImportNode; // Forward declaration

// The ImportProcessor is responsible for handling all import-related logic
// after the initial AST has been built. It iterates through the AST, finds
// ImportNode instances, and uses the ModuleResolver and Loader to load and
// integrate the imported content.
class ImportProcessor {
public:
    explicit ImportProcessor(Parser& parser);

    // Processes a vector of AST nodes, handling any ImportNodes it finds.
    // It will modify the AST in-place, replacing ImportNodes with the
    // content they represent or populating managers with the imported data.
    void process(std::vector<std::unique_ptr<BaseNode>>& nodes);

private:
    Parser& parser;

    // Private helper methods to handle different import types.
    void handleImportNode(ImportNode& node);
    void handleChtlImport(ImportNode& node);
    void handleNonChtlImport(ImportNode& node);
};