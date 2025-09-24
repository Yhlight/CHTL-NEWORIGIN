#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

// FIX: Forward declarations must be at the namespace level, not inside the class.
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

    std::string generate(const BaseNode& root);

private:
    void generateNode(const BaseNode& node);
    void generateElement(const ElementNode& node);
    void generateText(const TextNode& node);

    std::stringstream _output;
    int _indent_level = 0;

    void indent();
};

} // namespace CHTL
