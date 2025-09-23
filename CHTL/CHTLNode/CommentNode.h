#pragma once

#include "BaseNode.h"
#include <string>

struct CommentNode : public BaseNode {
    std::string content;
    bool isGeneratorAware;

    CommentNode(const std::string& content, bool isGeneratorAware)
        : BaseNode(NodeType::Comment), content(content), isGeneratorAware(isGeneratorAware) {}
};
