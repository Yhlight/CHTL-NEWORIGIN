#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual std::string toString() const = 0;
};

} // CHTL
