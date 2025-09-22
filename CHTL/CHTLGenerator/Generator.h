#pragma once

#include "../CHTLNode/Nodes.h"
#include <string>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator() = default;
    std::string Generate(const ElementNode& root);

private:
    void generateNode(const BaseNode& node, std::stringstream& ss);
    // Overloads for specific node types
    void generateNode(const ElementNode& element, std::stringstream& ss);
    void generateNode(const TextNode& text, std::stringstream& ss);
    // Note: AttributeNode is handled within generateElement
};

} // CHTL
