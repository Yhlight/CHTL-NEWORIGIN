#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    TextNode(const std::string& text) : text_content(text) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string text_content;
};
