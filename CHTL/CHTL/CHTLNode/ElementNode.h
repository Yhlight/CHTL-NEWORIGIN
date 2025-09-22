#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include "TextNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
