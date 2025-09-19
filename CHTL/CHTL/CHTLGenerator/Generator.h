#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <string>
#include <sstream>

// The Generator class walks the AST using the Visitor pattern
// and generates an HTML string from it.
class Generator : public Visitor {
public:
    // Generates HTML from the given AST root node.
    std::string generate(BaseNode& root);

    // Visitor methods for each node type.
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;

private:
    std::stringstream output;
};

#endif // CHTL_GENERATOR_H
