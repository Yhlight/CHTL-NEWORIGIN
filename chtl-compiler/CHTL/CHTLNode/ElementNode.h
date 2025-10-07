#pragma once

#include "Node.h"
#include <string>

class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    std::string tagName;
    NodeList children;
};