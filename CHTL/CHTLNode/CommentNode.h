#pragma once

#include "BaseNode.h"
#include <string>

struct CommentNode : public BaseNode {
    std::string content;
    bool isGeneratorAware;

    CommentNode(const std::string& content, bool isGeneratorAware)
        : content(content), isGeneratorAware(isGeneratorAware) {}

    NodeType getType() const override { return NodeType::Comment; }
};
