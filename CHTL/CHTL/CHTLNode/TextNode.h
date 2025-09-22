#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(const std::string& content) : content(content) {}

    void accept(AstVisitor& visitor) override;
};

} // namespace CHTL
