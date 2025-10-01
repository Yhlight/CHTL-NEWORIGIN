#ifndef CHTL_EXPRESSION_EVALUATOR_H
#define CHTL_EXPRESSION_EVALUATOR_H

#include <string>
#include <memory>
#include "CHTLNode/ExpressionNode.h"

namespace CHTL {

class StyleNode;

struct EvaluatedValue {
    enum class Type { NUMBER, STRING };

    Type type;
    double number_value = 0.0;
    std::string string_value;
    std::string unit;

    bool operator==(const EvaluatedValue& other) const {
        if (type != other.type) return false;
        if (type == Type::NUMBER) {
            return number_value == other.number_value && unit == other.unit;
        }
        return string_value == other.string_value;
    }

    bool isTruthy() const {
        if (type == Type::NUMBER) {
            return number_value != 0;
        }
        return !string_value.empty();
    }
};

class ExpressionEvaluator {
public:
    EvaluatedValue evaluate(const ExpressionNode* expression, const BaseNode* root, const StyleNode* style_context = nullptr);
};

}

#endif // CHTL_EXPRESSION_EVALUATOR_H