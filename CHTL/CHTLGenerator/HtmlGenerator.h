#pragma once

#include "Visitor.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/RootNode.h"
#include <sstream>
#include <string>

class HtmlGenerator : public Visitor {
public:
    std::string generate(Node& root);

    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;

private:
    std::stringstream ss;
    int indentLevel = 0;

    void indent();
};
