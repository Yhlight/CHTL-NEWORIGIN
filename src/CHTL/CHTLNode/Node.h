#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <memory>

class Node {
public:
    virtual ~Node() = default;
    virtual std::string toString(int indent = 0) const = 0;
};

#endif // CHTL_NODE_H