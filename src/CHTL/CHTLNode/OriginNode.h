#pragma once

#include "BaseNode.h"
#include <string>

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& name, const std::string& content)
        : originType(type), name(name), content(content) {}

    NodeType getType() const override { return NodeType::Origin; }

    const std::string& getOriginType() const { return originType; }
    const std::string& getName() const { return name; }
    const std::string& getContent() const { return content; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<OriginNode>(originType, name, content);
    }

private:
    std::string originType; // e.g., "Html", "Style", "JavaScript"
    std::string name;
    std::string content;
};