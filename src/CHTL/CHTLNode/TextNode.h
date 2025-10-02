#pragma once

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    TextNode(const std::string& text) : text(text) {}

    const std::string& getText() const {
        return text;
    }

private:
    std::string text;
};