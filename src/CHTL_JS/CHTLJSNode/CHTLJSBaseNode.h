#pragma once

#include <string>

class CHTLJSBaseNode {
public:
    virtual ~CHTLJSBaseNode() = default;
    virtual std::string toString() const = 0;
};