#ifndef CHTL_EXPRESSION_EVALUATOR_H
#define CHTL_EXPRESSION_EVALUATOR_H

#include <string>
#include <memory>
#include "CHTLNode/ExpressionNode.h"

namespace CHTL {

class StyleNode;

struct EvaluatedValue {
    enum class Type { NUMBER, STRING, BOOLEAN };

    Type type;
    double number_value = 0.0;
    std::string string_value;
    std::string unit;
    bool bool_value = false;

    bool operator==(const EvaluatedValue& other) const {
        if (type != other.type) return false;
        if (type == Type::NUMBER) {
            return number_value == other.number_value && unit == other.unit;
        }
        if (type == Type::BOOLEAN) {
            return bool_value == other.bool_value;
        }
        return string_value == other.string_value;
    }
};

class ExpressionEvaluator {
public:
    EvaluatedValue evaluate(const ExpressionNode* expression, const BaseNode* root, const StyleNode* style_context = nullptr);
};

}

#endif // CHTL_EXPRESSION_EVALUATOR_H