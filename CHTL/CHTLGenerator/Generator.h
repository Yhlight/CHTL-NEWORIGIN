#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <string>
#include <sstream>

class Generator : public AstVisitor {
public:
    std::string generate(BaseNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;

private:
    std::stringstream output;
    int indentLevel = 0;

    void indent();
};
