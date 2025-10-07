#pragma once

#include "Node.h"
#include <string>

class CommentNode : public Node {
public:
    CommentNode(const std::string& content) : content(content) {}

    std::string content;
};