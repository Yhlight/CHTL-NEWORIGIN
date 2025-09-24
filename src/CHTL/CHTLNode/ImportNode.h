#pragma once

#include "BaseNode.h"
#include <string>

/**
 * @brief Represents an [Import] statement in the AST.
 *
 * This node holds all the parsed information about an import statement.
 * The actual file loading and processing is deferred to a later compiler pass.
 */
class ImportNode : public BaseNode {
public:
    // The type of import, e.g., "Chtl", "Style", "Element"
    std::string importType;
    // The specific item to import, for precise imports.
    std::string itemName;
    // The path to the file to import from.
    std::string path;
    // The optional alias for the import.
    std::string alias;

    ImportNode(std::string importType, std::string itemName, std::string path, std::string alias)
        : importType(std::move(importType)),
          itemName(std::move(itemName)),
          path(std::move(path)),
          alias(std::move(alias)) {}

    NodeType getType() const override {
        return NodeType::Import;
    }
};
