#pragma once

#include <memory>
#include <vector>

class Node {
public:
    virtual ~Node() = default;
};

using NodePtr = std::shared_ptr<Node>;
using NodeList = std::vector<NodePtr>;