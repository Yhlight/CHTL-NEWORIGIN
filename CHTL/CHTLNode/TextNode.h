#pragma once

#include "BaseNode.h"
#include <string>
#include <utility>

namespace CHTL {

class TextNode : public BaseNode {
public:
    explicit TextNode(std::string content) : content(std::move(content)) {}

    // The text content of the node.
    std::string content;

    // Implementation of the accept method for the Visitor pattern.
    void accept(NodeVisitor& visitor) override;
};

} // namespace CHTL
