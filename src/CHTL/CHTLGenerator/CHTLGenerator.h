#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class ElementNode;
class TextNode;
class SelectorBlockNode;

class CHTLGenerator {
public:
    CHTLGenerator() = default;

    std::string generate(BaseNode& root);
    std::string getGlobalStyles() const;

private:
    void generateNode(BaseNode& node);
    void generateElement(ElementNode& node);
    void generateText(TextNode& node);
    void generateSelectorBlock(const SelectorBlockNode& node);

    std::stringstream _output;
    std::stringstream _global_styles;
    int _indent_level = 0;

    void indent();
};

} // namespace CHTL
