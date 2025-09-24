#pragma once

#include "BaseNode.h"
#include <string>
#include <unordered_map>
#include <utility>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    explicit ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

    // The tag name of the element (e.g., "div", "span").
    std::string tagName;

    // A map to store the element's attributes (e.g., id="box", class="main").
    std::unordered_map<std::string, std::string> attributes;

    // Implementation of the accept method for the Visitor pattern.
    void accept(NodeVisitor& visitor) override;
};

} // namespace CHTL
