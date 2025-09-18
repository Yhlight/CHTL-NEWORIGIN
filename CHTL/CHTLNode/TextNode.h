#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// An AST node representing a text block.
class TextNode : public BaseNode {
public:
    std::string content;
};

} // namespace CHTL
