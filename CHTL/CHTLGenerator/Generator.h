#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/Context.h"
#include <string>
#include <vector>
#include <sstream>

// The Generator implements the NodeVisitor pattern to traverse the AST
// and generate the final HTML output.
class CHTLGenerator : public NodeVisitor {
public:
    void generate(const std::vector<std::shared_ptr<BaseNode>>& nodes, CHTLContext& context);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;

private:
    CHTLContext* context = nullptr;
    int indent_level = 0;
    void indent();
};
