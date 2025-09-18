#pragma once

#include "../../CHTL/CHTLNode/ASTVisitor.h"
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class ASTPrinter : public ASTVisitor {
public:
    std::string print(NodePtr& node);

    std::string visit(ElementNode& node) override;
    std::string visit(TextNode& node) override;
    std::string visit(CommentNode& node) override;
    std::string visit(AttributeNode& node) override;
    std::string visit(StyleNode& node) override;
};

} // namespace CHTL
