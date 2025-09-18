#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <sstream>

// Forward declare node types to avoid circular dependencies if they were in separate files.
class ElementNode;
class TextNode;

class Generator {
public:
    Generator() = default;

    // generate is the main entry point. It takes the root of the AST
    // and returns the generated output string.
    std::string generate(NodePtr node);

private:
    // The main visit method that dispatches to the correct node-specific visitor.
    void visit(NodePtr node);

    // Specific visitors for each node type.
    void visitElementNode(ElementNode* node);
    void visitTextNode(TextNode* node);

    // A stringstream to efficiently build the output string.
    std::stringstream m_out;
};
