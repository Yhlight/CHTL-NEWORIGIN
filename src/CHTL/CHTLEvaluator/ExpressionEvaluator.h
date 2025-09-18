#pragma once

#include "../CHTLNode/AstVisitor.h"
#include "../CHTLNode/Expression/ExprNode.h"
#include <string>
#include <any>

namespace CHTL
{
    // A struct to hold the result of an expression evaluation.
    // It can hold different types of values (e.g., number, string) and a unit.
    struct EvaluatedValue
    {
        double numericValue = 0.0;
        std::string stringValue;
        std::string unit;
        bool isNumeric = false;
    };

    // ExpressionEvaluator traverses an expression AST and computes its value.
    class ExpressionEvaluator : public AstVisitor
    {
    public:
        EvaluatedValue evaluate(ExprNode &node);

        // Expression node visitors
        void visit(LiteralExprNode &node) override;
        void visit(BinaryOpExprNode &node) override;
        void visit(PropertyAccessExprNode &node) override;
        void visit(GroupExprNode &node) override;

        // Non-expression node visitors (not used by this visitor)
        void visit(ElementNode &node) override {}
        void visit(TextNode &node) override {}
        void visit(CommentNode &node) override {}
        void visit(StyleNode &node) override {}

    private:
        EvaluatedValue lastValue;
    };
}
