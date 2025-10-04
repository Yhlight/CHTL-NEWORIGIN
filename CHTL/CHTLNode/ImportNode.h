#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class ImportType {
    HTML,
    STYLE,
    JAVASCRIPT,
    CHTL,
    CJMOD,
    CONFIG,
    UNKNOWN
};

enum class ImportCategory {
    CUSTOM,
    TEMPLATE,
    ORIGIN,
    CONFIGURATION,
    NONE
};

class ImportNode : public BaseNode {
public:
    ImportNode(ImportType importType, const std::string& path, const std::string& alias = "")
        : importType(importType), path(path), alias(alias), category(ImportCategory::NONE), itemType(""), itemName("") {
        type = NodeType::NODE_IMPORT;
    }

    ImportNode(ImportCategory category, const std::string& itemType, const std::string& itemName, const std::string& path, const std::string& alias = "")
        : importType(ImportType::CHTL), category(category), itemType(itemType), itemName(itemName), path(path), alias(alias) {
        type = NodeType::NODE_IMPORT;
    }

    ImportType getImportType() const { return importType; }
    ImportCategory getCategory() const { return category; }
    const std::string& getItemType() const { return itemType; }
    const std::string& getItemName() const { return itemName; }
    const std::string& getPath() const { return path; }
    const std::string& getAlias() const { return alias; }

private:
    ImportType importType;
    ImportCategory category;
    std::string itemType;
    std::string itemName;
    std::string path;
    std::string alias;
};

}