#include "ExpressionEvaluator.h"
#include "../CHTLNode/Expression/LiteralExprNode.h"
#include "../CHTLNode/Expression/BinaryOpExprNode.h"
#include "../CHTLNode/Expression/PropertyAccessExprNode.h"
#include "../CHTLNode/Expression/GroupExprNode.h"
#include <stdexcept>
#include <charconv> // For std::from_chars

namespace CHTL
{
    // Helper to parse a literal string like "100px" into a value and a unit.
    EvaluatedValue parseLiteral(const std::string& literal) {
        EvaluatedValue val;
        double num;
        auto [ptr, ec] = std::from_chars(literal.data(), literal.data() + literal.size(), num);

        if (ec == std::errc()) {
            val.isNumeric = true;
            val.numericValue = num;
            val.unit = std::string(ptr, literal.data() + literal.size() - ptr);
        } else {
            val.isNumeric = false;
            val.stringValue = literal;
        }
        return val;
    }

    EvaluatedValue ExpressionEvaluator::evaluate(ExprNode &node)
    {
        node.accept(*this);
        return lastValue;
    }

    void ExpressionEvaluator::visit(LiteralExprNode &node)
    {
        lastValue = parseLiteral(node.literal.lexeme);
    }

    void ExpressionEvaluator::visit(BinaryOpExprNode &node)
    {
        EvaluatedValue left = evaluate(*node.left);
        EvaluatedValue right = evaluate(*node.right);

        if (!left.isNumeric || !right.isNumeric) {
            throw std::runtime_error("Cannot perform arithmetic on non-numeric values.");
        }

        // Basic unit check: for now, either units must match or one can be empty.
        if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
            throw std::runtime_error("Incompatible units for arithmetic: '" + left.unit + "' and '" + right.unit + "'.");
        }

        EvaluatedValue result;
        result.isNumeric = true;
        result.unit = !left.unit.empty() ? left.unit : right.unit;

        switch(node.op.type) {
            case TokenType::TOKEN_PLUS:
                result.numericValue = left.numericValue + right.numericValue;
                break;
            case TokenType::TOKEN_MINUS:
                result.numericValue = left.numericValue - right.numericValue;
                break;
            case TokenType::TOKEN_STAR:
                result.numericValue = left.numericValue * right.numericValue;
                break;
            case TokenType::TOKEN_SLASH:
                if (right.numericValue == 0) throw std::runtime_error("Division by zero.");
                result.numericValue = left.numericValue / right.numericValue;
                break;
            default:
                throw std::runtime_error("Unsupported binary operator.");
        }

        lastValue = result;
    }

    void ExpressionEvaluator::visit(GroupExprNode &node)
    {
        lastValue = evaluate(*node.expression);
    }

    void ExpressionEvaluator::visit(PropertyAccessExprNode &node)
    {
        // Property access evaluation is complex and requires context of the whole AST.
        // This will be implemented in a later stage.
        throw std::runtime_error("Property access evaluation is not yet implemented.");
    }
}
