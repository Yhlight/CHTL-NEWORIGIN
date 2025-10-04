#pragma once

#include "BaseNode.h"
#include <string>

class ImportNode : public BaseNode {
public:
    ImportNode(
        const std::string& path,
        const std::string& alias = "",
        const std::string& category = "", // e.g., Custom, Template
        const std::string& type = "",     // e.g., @Element, @Style
        const std::string& name = ""      // e.g., Box
    ) : path(path), alias(alias), category(category), importType(type), name(name) {}

    NodeType getType() const override { return NodeType::Import; }

    const std::string& getPath() const { return path; }
    const std::string& getAlias() const { return alias; }
    const std::string& getCategory() const { return category; }
    const std::string& getImportType() const { return importType; }
    const std::string& getName() const { return name; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ImportNode>(path, alias, category, importType, name);
    }

private:
    std::string path;
    std::string alias;
    std::string category;
    std::string importType;
    std::string name;
};