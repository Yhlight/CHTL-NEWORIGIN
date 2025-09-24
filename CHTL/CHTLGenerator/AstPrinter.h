#ifndef CHTL_AST_PRINTER_H
#define CHTL_AST_PRINTER_H

#include "../CHTLNode/AstVisitor.h"
#include "../CHTLNode/Node.h"
#include <iostream>

namespace CHTL {

class AstPrinter : public AstVisitor {
public:
    void Print(Node* root);
    void Visit(ElementNode& node) override;
    void Visit(TextNode& node) override;
    void Visit(CommentNode& node) override;

private:
    void Indent();
    int indentLevel = 0;
};

} // namespace CHTL

#endif // CHTL_AST_PRINTER_H
