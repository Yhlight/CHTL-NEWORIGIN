#pragma once

#include <memory>
#include <string>
#include <map>

// Forward declaration
class ElementNode;

// Base class for all expression nodes.
class ExprNode {
public:
    virtual ~ExprNode() = default;
    // The evaluate method will need context, like the element's own attributes.
    // For now, we'll pass a map representing the element's style properties.
    virtual std::string evaluate(const std::map<std::string, std::string>& context) const = 0;
};

using ExprNodePtr = std::shared_ptr<ExprNode>;