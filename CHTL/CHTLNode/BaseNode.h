#ifndef BASE_NODE_H
#define BASE_NODE_H

#include <iostream>
#include <string>

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void print(int indent = 0) const = 0;
};

} // namespace CHTL

#endif // BASE_NODE_H