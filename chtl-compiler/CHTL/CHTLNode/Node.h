#pragma once

#include <memory>
#include <vector>

class Node {
public:
    virtual ~Node() = default;
    virtual std::shared_ptr<Node> clone() const = 0;
};

using NodePtr = std::shared_ptr<Node>;
using NodeList = std::vector<NodePtr>;