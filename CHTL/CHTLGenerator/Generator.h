#pragma once

#include "CHTL/CHTLNode/Node.h"
#include "CompilerDispatcher/CompilerDispatcher.h" // For CompilationResult
#include <string>
#include <map>

namespace CHTL {

class Generator {
public:
    CompilationResult generate(const ElementNode& root);

private:
    void generateNode(const Node& node, std::string& output, int indentLevel);
    void generateElement(const ElementNode& element, std::string& output, int indentLevel);
    void generateText(const TextNode& text, std::string& output, int indentLevel);
    void generateStyleBlock(const StyleBlockNode& style_block, std::string& output, int indentLevel);

    // State for the generator
    CompilationResult m_result;
    int m_placeholder_count = 0;
};

} // namespace CHTL
