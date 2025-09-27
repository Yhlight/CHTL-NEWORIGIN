#include "StyleValue.h"

// --- DynamicConditionalExpression Implementation ---

DynamicConditionalExpression::DynamicConditionalExpression(const DynamicConditionalExpression& other)
    : selector(other.selector),
      property(other.property),
      op(other.op),
      value_to_compare(other.value_to_compare) {
    if (other.true_branch) {
        true_branch = std::make_unique<StyleValue>(*other.true_branch);
    }
    if (other.false_branch) {
        false_branch = std::make_unique<StyleValue>(*other.false_branch);
    }
}

DynamicConditionalExpression& DynamicConditionalExpression::operator=(const DynamicConditionalExpression& other) {
    if (this != &other) {
        selector = other.selector;
        property = other.property;
        op = other.op;
        value_to_compare = other.value_to_compare;
        if (other.true_branch) {
            true_branch = std::make_unique<StyleValue>(*other.true_branch);
        } else {
            true_branch.reset();
        }
        if (other.false_branch) {
            false_branch = std::make_unique<StyleValue>(*other.false_branch);
        } else {
            false_branch.reset();
        }
    }
    return *this;
}

StyleValue::StyleValue(StyleValue&& other) noexcept
    : type(other.type),
      numeric_val(other.numeric_val),
      unit(std::move(other.unit)),
      string_val(std::move(other.string_val)),
      bool_val(other.bool_val),
      responsive_var_name(std::move(other.responsive_var_name)),
      dynamic_expr(std::move(other.dynamic_expr)),
      dynamic_expression_str(std::move(other.dynamic_expression_str)) {
    other.type = EMPTY;
    other.numeric_val = 0.0;
    other.bool_val = false;
}

StyleValue& StyleValue::operator=(StyleValue&& other) noexcept {
    if (this != &other) {
        type = other.type;
        numeric_val = other.numeric_val;
        unit = std::move(other.unit);
        string_val = std::move(other.string_val);
        bool_val = other.bool_val;
        responsive_var_name = std::move(other.responsive_var_name);
        dynamic_expr = std::move(other.dynamic_expr);
        dynamic_expression_str = std::move(other.dynamic_expression_str);

        other.type = EMPTY;
        other.numeric_val = 0.0;
        other.bool_val = false;
    }
    return *this;
}

// --- StyleValue Implementation ---

StyleValue::StyleValue(ValueType t) : type(t) {}

StyleValue::StyleValue(const std::string& s) : type(STRING), string_val(s) {}

StyleValue::StyleValue(double val, const std::string& u) : type(NUMERIC), numeric_val(val), unit(u) {}

StyleValue::StyleValue(const StyleValue& other) : type(other.type) {
    switch (type) {
        case NUMERIC:
            numeric_val = other.numeric_val;
            unit = other.unit;
            break;
        case STRING:
            string_val = other.string_val;
            break;
        case BOOL:
            bool_val = other.bool_val;
            break;
        case RESPONSIVE:
            responsive_var_name = other.responsive_var_name;
            unit = other.unit;
            break;
        case DYNAMIC_CONDITIONAL:
            if (other.dynamic_expr) {
                dynamic_expr = std::make_shared<DynamicConditionalExpression>(*other.dynamic_expr);
            }
            break;
        case DYNAMIC_EXPRESSION:
            dynamic_expression_str = other.dynamic_expression_str;
            break;
        case EMPTY:
        case DELETED:
            // No data to copy
            break;
    }
}

StyleValue& StyleValue::operator=(const StyleValue& other) {
    if (this != &other) {
        type = other.type;
        // Reset all optional members first
        dynamic_expr.reset();

        switch (type) {
            case NUMERIC:
                numeric_val = other.numeric_val;
                unit = other.unit;
                break;
            case STRING:
                string_val = other.string_val;
                break;
            case BOOL:
                bool_val = other.bool_val;
                break;
            case RESPONSIVE:
                responsive_var_name = other.responsive_var_name;
                unit = other.unit;
                break;
            case DYNAMIC_CONDITIONAL:
                if (other.dynamic_expr) {
                    dynamic_expr = std::make_shared<DynamicConditionalExpression>(*other.dynamic_expr);
                }
            break;
        case DYNAMIC_EXPRESSION:
            dynamic_expression_str = other.dynamic_expression_str;
                break;
            case EMPTY:
            case DELETED:
                // No data to copy
                break;
        }
    }
    return *this;
}