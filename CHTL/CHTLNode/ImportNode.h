#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>

class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& fullType, const std::string& entityName, const std::string& filePath, const std::string& alias)
        : fullType(fullType), entityName(entityName), filePath(filePath), alias(alias) {}

    std::string toString(int depth = 0) const override {
        std::string indent(depth * 2, ' ');
        std::string result = indent + "ImportNode: " + fullType + " " + entityName + " from \"" + filePath + "\"";
        if (!alias.empty()) {
            result += " as " + alias;
        }
        result += "\n";
        return result;
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    const std::string& getFullType() const { return fullType; }
    const std::string& getEntityName() const { return entityName; }
    const std::string& getFilePath() const { return filePath; }
    const std::string& getAlias() const { return alias; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ImportNode>(fullType, entityName, filePath, alias);
    }

private:
    std::string fullType;
    std::string entityName;
    std::string filePath;
    std::string alias;
};

#endif // CHTL_IMPORT_NODE_H