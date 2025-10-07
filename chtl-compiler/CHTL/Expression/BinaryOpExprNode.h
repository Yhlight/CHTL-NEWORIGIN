#pragma once

#include "ExprNode.h"
#include <regex>
#include <string>

class BinaryOpExprNode : public ExprNode {
public:
    BinaryOpExprNode(ExprNodePtr left, const std::string& op, ExprNodePtr right)
        : left(left), op(op), right(right) {}

    std::string evaluate(const std::map<std::string, std::string>& context) const override {
        std::string left_val_str = left->evaluate(context);
        std::string right_val_str = right->evaluate(context);

        std::regex re("(-?[0-9]+\\.?[0-9]*)(.*)");
        std::smatch left_match, right_match;
        std::regex_match(left_val_str, left_match, re);
        std::regex_match(right_val_str, right_match, re);

        double left_num = left_match.empty() ? 0 : std::stod(left_match[1].str());
        std::string left_unit = left_match.empty() ? "" : left_match[2].str();
        double right_num = right_match.empty() ? 0 : std::stod(right_match[1].str());
        std::string right_unit = right_match.empty() ? "" : right_match[2].str();

        bool result = false;
        if (op == ">") result = left_num > right_num;
        else if (op == "<") result = left_num < right_num;
        else if (op == ">=") result = left_num >= right_num;
        else if (op == "<=") result = left_num <= right_num;
        else if (op == "==") result = left_num == right_num;
        else if (op == "!=") result = left_num != right_num;
        else if (op == "&&") result = (left_num != 0) && (right_num != 0);
        else if (op == "||") result = (left_num != 0) || (right_num != 0);
        else { // Arithmetic
            if (!left_unit.empty() && !right_unit.empty() && left_unit != right_unit) {
                throw std::runtime_error("Mismatched units in expression: " + left_unit + " and " + right_unit);
            }
            std::string result_unit = !left_unit.empty() ? left_unit : right_unit;
            double result_val = 0;

            if (op == "+") result_val = left_num + right_num;
            else if (op == "-") result_val = left_num - right_num;
            else if (op == "*") result_val = left_num * right_num;
            else if (op == "/") {
                if (right_num == 0) throw std::runtime_error("Division by zero.");
                result_val = left_num / right_num;
            }

            std::string result_str = std::to_string(result_val);
            result_str.erase(result_str.find_last_not_of('0') + 1, std::string::npos);
            if (result_str.back() == '.') result_str.pop_back();
            return result_str + result_unit;
        }

        return result ? "1" : "0";
    }

private:
    ExprNodePtr left;
    std::string op;
    ExprNodePtr right;
};