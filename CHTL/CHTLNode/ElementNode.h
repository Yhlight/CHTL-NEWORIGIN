#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tag_name) : tag_name(tag_name) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string tag_name;
    std::map<std::string, std::string> attributes;
};
