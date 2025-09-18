#pragma once

#include "../../CHTL/CHTLNode/AstVisitor.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/Expression/LiteralExprNode.h"
#include "../../CHTL/CHTLNode/Expression/BinaryOpExprNode.h"
#include "../../CHTL/CHTLNode/Expression/PropertyAccessExprNode.h"
#include "../../CHTL/CHTLNode/Expression/GroupExprNode.h"


namespace CHTL
{
    // AstPrinter is a concrete implementation of the AstVisitor that prints
    // a representation of the AST to the console.
    class AstPrinter : public AstVisitor
    {
    public:
        void print(const std::vector<std::unique_ptr<BaseNode>> &nodes);

        void visit(ElementNode &node) override;
        void visit(TextNode &node) override;
        void visit(CommentNode &node) override;
        void visit(StyleNode &node) override;
        void visit(LiteralExprNode &node) override;
        void visit(BinaryOpExprNode &node) override;
        void visit(PropertyAccessExprNode &node) override;
        void visit(GroupExprNode &node) override;

    private:
        void indent();
        void unindent();
        std::string indentation;
    };
}
