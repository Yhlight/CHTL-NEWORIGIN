#pragma once

#include "../CHTLNode/AstVisitor.h"
#include "../CHTLNode/Expression/ExprNode.h"
#include <string>
#include <any>
#include <vector>
#include <memory>

namespace CHTL
{
    // Forward declare node type used in a function signature
    class ElementNode;

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
        // The AST context is needed to resolve property references (e.g., box.width)
        EvaluatedValue evaluate(ExprNode &node, const std::vector<std::unique_ptr<BaseNode>>* astContext, int depth = 0);

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
        ElementNode* findNode(const std::string& selector, const std::vector<std::unique_ptr<BaseNode>>& nodes);

        EvaluatedValue lastValue;
        const std::vector<std::unique_ptr<BaseNode>>* astContext = nullptr;
        int currentDepth = 0;
    };
}
