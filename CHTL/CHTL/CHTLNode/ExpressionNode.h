#pragma once

#include <memory>
#include <string>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// --- Start of merged Value.h content ---
enum class ValueType {
    NUMBER,
    STRING
};

struct Value {
    ValueType type;
    double number_val = 0.0;
    std::string string_val;
    std::string unit;

    static Value make_number(double num, const std::string& u) {
        Value v;
        v.type = ValueType::NUMBER;
        v.number_val = num;
        v.unit = u;
        return v;
    }

    static Value make_string(const std::string& str) {
        Value v;
        v.type = ValueType::STRING;
        v.string_val = str;
        return v;
    }
};
// --- End of merged Value.h content ---


// Forward declare all expression node types and the visitor
class BinaryOpNode;
class LiteralNode;
class ExpressionVisitor;

// Base class for all expression nodes
class ExpressionNode {
public:
    virtual ~ExpressionNode() = default;
    virtual Value accept(ExpressionVisitor& visitor) = 0; // Visitor for evaluation
};

// The Visitor interface for expressions
class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual Value visit(BinaryOpNode& node) = 0;
    virtual Value visit(LiteralNode& node) = 0;
};

} // namespace CHTL
