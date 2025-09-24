#include "AstPrinter.h"
#include "../CHTL/CHTLNode/CHTLNode.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/CommentNode.h"

std::string AstPrinter::print(CHTLNode& node) {
    node.accept(*this);
    return result;
}

void AstPrinter::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        result += "  ";
    }
}

void AstPrinter::visit(ElementNode& node) {
    indent();
    result += "Element(" + node.getTagName() + ")\n";
    indentLevel++;
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    indentLevel--;
}

void AstPrinter::visit(TextNode& node) {
    indent();
    result += "Text(\"" + node.getContent() + "\")\n";
}

void AstPrinter::visit(CommentNode& node) {
    indent();
    result += "Comment(\"" + node.getContent() + "\")\n";
}
