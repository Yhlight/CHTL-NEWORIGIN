#pragma once

#include "BaseNode.h"
#include <string>

class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& type, const std::string& path, const std::string& alias = "")
        : importType(type), path(path), alias(alias) {}

    NodeType getType() const override { return NodeType::Import; }

    const std::string& getImportType() const { return importType; }
    const std::string& getPath() const { return path; }
    const std::string& getAlias() const { return alias; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ImportNode>(importType, path, alias);
    }

private:
    std::string importType; // e.g., "Chtl", "Html", "Style"
    std::string path;
    std::string alias;
};