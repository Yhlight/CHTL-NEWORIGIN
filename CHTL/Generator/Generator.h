#pragma once

#include "CHTL/AstVisitor.h"
#include "CHTL/Node/Node.h"
#include <string>
#include <sstream>

class Generator : public AstVisitor {
public:
    std::string generate(const NodeList& ast);

    void visit(const ElementNode& node) override;
    void visit(const TextNode& node) override;
    void visit(const CommentNode& node) override;

private:
    std::stringstream output;
    int indent_level = 0;
    void indent();
};
