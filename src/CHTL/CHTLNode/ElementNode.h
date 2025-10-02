#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    const std::string& getTagName() const {
        return tagName;
    }

    void setAttribute(const std::string& name, const std::string& value) {
        attributes[name] = value;
    }

    const std::map<std::string, std::string>& getAttributes() const {
        return attributes;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
};