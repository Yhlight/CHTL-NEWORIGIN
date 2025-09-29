#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

// Forward declaration
class CHTLGenerator;

// Represents the main category of an import statement, e.g., [Import] @Chtl, [Import] [Custom] ...
enum class ImportCategory {
    Chtl,
    Html,
    Style,
    JavaScript,
    CJmod,
    Config,
    Template,
    Custom,
    Origin,
};

// Represents the specific type within a category, e.g. @Element, @Style
enum class ImportType {
    None,
    Element,
    Style,
    Var,
    Config,
    Html,
    JavaScript,
    // Support for other custom origin types can be added here
};


class ImportNode : public BaseNode {
public:
    ImportCategory category;
    ImportType type;
    std::string itemName; // Specific item to import, e.g., "MyComponent"
    std::string path;
    std::string alias;

    ImportNode(ImportCategory category, ImportType type, const std::string& path,
               const std::string& itemName = "", const std::string& alias = "")
        : category(category), type(type), path(path), itemName(itemName), alias(alias) {}

    NodeType getType() const override { return NodeType::Import; }
};