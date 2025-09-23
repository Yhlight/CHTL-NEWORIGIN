#pragma once

#include "BaseNode.h"
#include <iostream>
#include <string>
#include <vector>

// Represents the type of import, e.g., [Custom] @Element
struct ImportType {
    bool is_custom = false;
    bool is_template = false;
    bool is_origin = false;
    bool is_config = false;
    std::string type_name; // e.g., @Element, @Style, @Chtl
};

// Represents an [Import] directive in CHTL
class ImportNode : public BaseNode {
public:
    ImportType import_type;
    std::string item_name; // The specific item to import, e.g., "MyElement"
    std::string path;
    std::string alias;

    ImportNode() : BaseNode(NodeType::Import) {}

    void print(int level = 0) const override {
        std::cout << std::string(level * 2, ' ') << "ImportNode:\n";
        std::cout << std::string((level + 1) * 2, ' ') << "Type: " << import_type.type_name;
        if (import_type.is_custom) std::cout << " (Custom)";
        if (import_type.is_template) std::cout << " (Template)";
        std::cout << "\n";
        if (!item_name.empty()) {
            std::cout << std::string((level + 1) * 2, ' ') << "Item: " << item_name << "\n";
        }
        std::cout << std::string((level + 1) * 2, ' ') << "Path: " << path << "\n";
        if (!alias.empty()) {
            std::cout << std::string((level + 1) * 2, ' ') << "Alias: " << alias << "\n";
        }
    }
};
