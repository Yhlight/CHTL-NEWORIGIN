#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <string>
#include <vector>
#include <memory>

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual std::string toString(int depth = 0) const = 0;
};

#endif //CHTL_BASE_NODE_H