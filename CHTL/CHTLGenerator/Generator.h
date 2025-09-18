#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace CHTL {

class Generator {
public:
    Generator();

    // The main entry point. Takes the root of the AST and returns the generated HTML.
    std::string generate(const ElementNode& root);

private:
    // Visitor methods for different node types.
    void visit(const BaseNode& node);
    void visitElement(const ElementNode& node);
    void visitText(const TextNode& node);
    void visitComment(const CommentNode& node);

    void indent();

    std::stringstream output;
    int indentLevel = 0;
};

} // namespace CHTL
