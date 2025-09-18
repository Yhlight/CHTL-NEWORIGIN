#pragma once

#include "../../CHTL/CHTLNode/ASTVisitor.h"
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <sstream>
#include <vector>

namespace CHTL {

class ASTPrinter : public ASTVisitor {
public:
    std::string print(NodePtr& node);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(AttributeNode& node) override;

private:
    std::stringstream out;
    void parenthesize(const std::string& name, const std::vector<NodePtr>& nodes);
};

} // namespace CHTL
