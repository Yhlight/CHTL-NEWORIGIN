#pragma once

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <sstream>
#include <set>

namespace CHTL {

// The CHTLGenerator class implements the NodeVisitor interface to traverse the AST
// and generate an HTML string.
class CHTLGenerator : public NodeVisitor {
public:
    CHTLGenerator();

    // The main entry point to generate code from an AST root node.
    std::string generate(const BaseNode& root);

    // Visitor methods for each concrete node type.
    void visit(const ElementNode& node) override;
    void visit(const TextNode& node) override;
    void visit(const CommentNode& node) override;

private:
    void indent();

    std::stringstream m_output;
    int m_indentationLevel = 0;
    // Set of HTML void elements that don't need a closing tag.
    std::set<std::string> m_voidElements;
};

} // namespace CHTL
