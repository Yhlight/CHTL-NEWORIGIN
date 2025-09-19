#pragma once

#include "BaseNode.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// An AST node representing an element (like <div> or <span>).
// It contains the element's tag name, a map of its attributes,
// a map of its style property expressions, and a vector of its child nodes.
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::vector<Token>> styleProperties;
    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL
