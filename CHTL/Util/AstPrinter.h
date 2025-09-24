#ifndef CHTL_AST_PRINTER_H
#define CHTL_AST_PRINTER_H

#include "AstVisitor.h"
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class CHTLNode;
class ElementNode;
class TextNode;

class AstPrinter : public AstVisitor {
public:
    std::string print(CHTLNode& node);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;

private:
    std::string result;
    int indentLevel = 0;
    void indent();
};

#endif // CHTL_AST_PRINTER_H
