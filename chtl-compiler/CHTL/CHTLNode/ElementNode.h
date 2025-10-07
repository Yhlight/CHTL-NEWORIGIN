#pragma once

#include "Node.h"
#include <string>
#include <map>

class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    std::string tagName;
    std::map<std::string, std::string> attributes;
    NodeList children;
};