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

class ImportNode : public BaseNode {
public:
    ImportNode(ImportType importType, const std::string& path, const std::string& alias = "")
        : importType(importType), path(path), alias(alias) {
        type = NodeType::NODE_IMPORT;
    }

    ImportType getImportType() const { return importType; }
    const std::string& getPath() const { return path; }
    const std::string& getAlias() const { return alias; }

private:
    ImportType importType;
    std::string path;
    std::string alias;
};

}