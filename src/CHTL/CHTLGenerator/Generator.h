#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/AttributeNode.h"
#include <string>
#include <sstream>

class Generator : public NodeVisitor {
public:
    std::string generate(BaseNode& root);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(AttributeNode& node) override;

private:
    std::stringstream output;
    int indentLevel = 0;

    void indent();
};