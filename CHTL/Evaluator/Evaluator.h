#pragma once

#include "CHTL/Expression/Expression.h"
#include "CHTL/Node/Node.h"
#include "CHTL/Node/ElementNode.h"
#include "Value.h"

class Evaluator : public ExprVisitor {
public:
    Evaluator(const NodeList& ast_root, const ElementNode& current_element);

    Value evaluate(const Expr& expr);

    // Visitor methods
    Value visit(const BinaryExpr& expr) override;
    Value visit(const UnaryExpr& expr) override;
    Value visit(const LiteralExpr& expr) override;
    Value visit(const ReferenceExpr& expr) override;
    Value visit(const ConditionalExpr& expr) override;
    Value visit(const LogicalExpr& expr) override;

private:
    const NodeList& ast_root;
    const ElementNode& current_element;

    // Helper to check for truthiness in CHTL
    bool is_truthy(const Value& value) const;
};
