#pragma once

#include "BaseNode.h"
#include <string>

struct TextNode : public BaseNode {
    std::string content;

    explicit TextNode(const std::string& content)
        : BaseNode(NodeType::Text), content(content) {}
};
