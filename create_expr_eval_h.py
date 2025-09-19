import os

file_path = "CHTL/CHTLGenerator/ExpressionEvaluator.h"
content = r"""
#pragma once

#include "../CHTLParser/Expr.h"
#include <string>
#include <any>

namespace CHTL {

// Represents a computed value from an expression.
struct EvaluatedValue {
    enum class Type { NUMBER, STRING, BOOL, NIL } type = Type::NIL;
    double number_val = 0.0;
    std::string string_val = "";
    bool bool_val = false;

    bool isTruthy() const {
        if (type == Type::NIL) return false;
        if (type == Type::BOOL) return bool_val;
        return true;
    }
};

// An interpreter for the expression AST.
class ExpressionEvaluator : public ExprVisitor {
public:
    ExpressionEvaluator();
    EvaluatedValue evaluate(const Expr& expr);

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitTernaryExpr(const TernaryExpr& expr) override;

private:
    EvaluatedValue evaluateAny(const std::any& anyValue);
    void checkNumberOperands(const Token& op, const EvaluatedValue& left, const EvaluatedValue& right);
};

} // namespace CHTL
"""

dir_name = os.path.dirname(file_path)
if not os.path.exists(dir_name):
    os.makedirs(dir_name)

with open(file_path, "w") as f:
    f.write(content.strip())

print(f"File '{file_path}' created successfully.")
