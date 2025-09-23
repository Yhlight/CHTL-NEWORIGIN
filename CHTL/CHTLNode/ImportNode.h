#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <optional>
#include <iostream>

enum class ImportType {
    Html,
    Style,
    JavaScript,
    Chtl,
    Config,
    CJmod
};

enum class ImportCategory {
    None,
    Custom,
    Template,
    Origin,
    Configuration
};

class ImportNode : public BaseNode {
public:
    ImportType type;
    ImportCategory category = ImportCategory::None;
    std::optional<std::string> importedItem; // e.g., "Box" in [Import] [Custom] @Element Box
    std::string path;
    std::optional<std::string> alias;

    ImportNode(ImportType type, const std::string& path)
        : type(type), path(path) {}

    NodeType getType() const override { return NodeType::Import; }

    virtual void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "ImportNode (";

        if (category != ImportCategory::None) {
            std::cout << "Category: " << static_cast<int>(category) << ", ";
        }

        std::cout << "Type: " << static_cast<int>(type);

        if (importedItem) {
            std::cout << ", Item: " << *importedItem;
        }

        std::cout << ", Path: \"" << path << "\"";

        if (alias) {
            std::cout << ", Alias: " << *alias;
        }

        std::cout << ")" << std::endl;
    }
};
