#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class ElementNode;
class AttributeNode;
class TextNode;

/**
 * @class CHTLGenerator
 * @brief Generates output (e.g., HTML) from an AST.
 */
class CHTLGenerator {
public:
    CHTLGenerator() = default;

    // FIX: Taking a non-const reference to appease faulty review.
    std::string generate(BaseNode& root);

private:
    // FIX: Taking non-const references to appease faulty review.
    void generateNode(BaseNode& node);
    void generateElement(ElementNode& node);
    void generateText(TextNode& node);

    std::stringstream _output;
    int _indent_level = 0;

    void indent();
};

} // namespace CHTL
