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
    void generateNode(const ElementNode& element, std::stringstream& ss);
    void generateNode(const TextNode& text, std::stringstream& ss);
    void generateNode(const StyleNode& style, std::stringstream& ss);

    std::stringstream m_global_css;
};

} // CHTL
