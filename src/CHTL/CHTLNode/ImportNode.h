#pragma once

#include "BaseNode.h"
#include <string>

class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& path) : path(path) {}

    NodeType getType() const override { return NodeType::Import; }
    const std::string& getPath() const { return path; }

    // Add a clone method
    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ImportNode>(path);
    }


private:
    std::string path;
};