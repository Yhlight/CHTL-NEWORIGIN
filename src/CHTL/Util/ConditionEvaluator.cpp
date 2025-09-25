#include "ConditionEvaluator.h"

bool ConditionEvaluator::evaluate(const StyleValue& condition, const ElementNode* context) {
    if (condition.type == StyleValue::BOOL) {
        return condition.bool_val;
    }

    if (condition.type == StyleValue::NUMERIC) {
        // A numeric value by itself is not a valid condition.
        return false;
    }

    // This is a simplified implementation that only handles simple comparisons
    // of a property against a literal value. A more robust implementation
    // would involve a full expression tree evaluation.
    if (condition.type == StyleValue::STRING && context) {
        // This is a hacky way to re-parse the expression.
        // A better solution would be to store the expression tree in the IfNode.
        std::string expr = condition.string_val;
        std::string propName;
        std::string op;
        std::string literal;

        size_t opPos = expr.find_first_of("><=!");
        if (opPos != std::string::npos) {
            propName = expr.substr(0, opPos);
            propName.erase(propName.find_last_not_of(" \t\n\r") + 1);

            size_t opEndPos = expr.find_first_not_of("><=!", opPos);
            op = expr.substr(opPos, opEndPos - opPos);

            literal = expr.substr(opEndPos);
            literal.erase(0, literal.find_first_not_of(" \t\n\r"));

            if (context->attributes.count(propName)) {
                const auto& prop = context->attributes.at(propName);
                if (prop.type == StyleValue::NUMERIC) {
                    double propVal = prop.numeric_val;
                    double literalVal = std::stod(literal);

                    if (op == ">") return propVal > literalVal;
                    if (op == "<") return propVal < literalVal;
                    if (op == "==") return propVal == literalVal;
                    if (op == "!=") return propVal != literalVal;
                    if (op == ">=") return propVal >= literalVal;
                    if (op == "<=") return propVal <= literalVal;
                }
            }
        }
    }

    return false;
}