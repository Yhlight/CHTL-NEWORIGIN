#include "AstPrinter.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

namespace CHTL {

void AstPrinter::Print(Node* root) {
    if (root) {
        root->Accept(*this);
    }
}

void AstPrinter::Indent() {
    for (int i = 0; i < indentLevel; ++i) {
        std::cout << "  ";
    }
}

void AstPrinter::Visit(ElementNode& node) {
    Indent();
    std::cout << "<" << node.tagName << ">" << std::endl;
    indentLevel++;
    for (const auto& child : node.children) {
        child->Accept(*this);
    }
    indentLevel--;
}

void AstPrinter::Visit(TextNode& node) {
    Indent();
    std::cout << "Text: \"" << node.text << "\"" << std::endl;
}

void AstPrinter::Visit(CommentNode& node) {
    Indent();
    std::cout << "Comment: \"" << node.text << "\"" << std::endl;
}

} // namespace CHTL
