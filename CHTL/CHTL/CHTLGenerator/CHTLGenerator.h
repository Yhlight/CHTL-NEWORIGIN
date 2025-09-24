#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../../Util/AstVisitor.h"
#include <string>

// Forward declarations
class CHTLNode;
class ElementNode;
class TextNode;

class CHTLGenerator : public AstVisitor {
public:
    std::string generate(CHTLNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;

private:
    std::string result;
};

#endif // CHTL_GENERATOR_H
