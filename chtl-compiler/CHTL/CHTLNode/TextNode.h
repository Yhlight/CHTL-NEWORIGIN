#pragma once

#include "Node.h"
#include <string>

class TextNode : public Node {
public:
    TextNode(const std::string& content) : content(content) {}

    std::string content;

    NodePtr clone() const override {
        return std::make_shared<TextNode>(content);
    }
};