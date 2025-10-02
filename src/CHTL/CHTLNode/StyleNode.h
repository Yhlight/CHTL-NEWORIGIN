#pragma once

#include "BaseNode.h"
#include <string>

class StyleNode : public BaseNode {
public:
    StyleNode(const std::string& style) : style(style) {}

    NodeType getType() const override { return NodeType::Style; }

    const std::string& getStyle() const {
        return style;
    }

private:
    std::string style;
};