#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    TextNode(const std::string& content) : content(content) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string content;
};