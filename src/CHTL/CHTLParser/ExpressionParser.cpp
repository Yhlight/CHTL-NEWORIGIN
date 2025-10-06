#include "ExpressionParser.h"
#include "../../Util/StringUtil/StringUtil.h"
#include "../../SharedCore/SaltBridge.h"
#include <cmath>
#include <sstream>
#include <unordered_set>

namespace CHTL {

// CssUnit实现
double CssUnit::toDouble() const {
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

// PropertyExpression实现
String PropertyExpression::evaluate() const {
    // 这里需要从上下文获取属性值，暂时返回占位符
    return "var(--" + property_ + ")";
}

// BinaryExpression实现
String BinaryExpression::evaluate() const {
    CssUnit result = evaluateNumeric();
    return result.toString();
}

CssUnit BinaryExpression::evaluateNumeric() const {
    // 获取左右操作数
    String leftValue = left_->evaluate();
    String rightValue = right_->evaluate();
    
    // 解析为CssUnit
    auto parseUnit = [](const String& str) -> CssUnit {
        String numStr;
        String unit;
        bool foundNum = false;
        
        for (size_t i = 0; i < str.length(); ++i) {
            char c = str[i];
            if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
                numStr += c;
                foundNum = true;
            } else if (foundNum) {
                unit = str.substr(i);
                break;
            }
        }
        
        return CssUnit(numStr, unit);
    };
    
    CssUnit left = parseUnit(leftValue);
    CssUnit right = parseUnit(rightValue);
    
    // 检查单位是否兼容
    String resultUnit = left.unit;
    if (!left.unit.empty() && !right.unit.empty() && left.unit != right.unit) {
        // 不同单位，尝试使用calc()
        std::stringstream ss;
        ss << "calc(" << left.toString() << " ";
        switch (op_) {
            case OperatorType::Add: ss << "+"; break;
            case OperatorType::Subtract: ss << "-"; break;
            case OperatorType::Multiply: ss << "*"; break;
            case OperatorType::Divide: ss << "/"; break;
            default: break;
        }
        ss << " " << right.toString() << ")";
        return CssUnit(ss.str(), "");
    }
    
    // 如果右侧有单位但左侧没有，使用右侧单位
    if (left.unit.empty() && !right.unit.empty()) {
        resultUnit = right.unit;
    }
    
    // 执行运算
    double leftNum = left.toDouble();
    double rightNum = right.toDouble();
    double result = 0.0;
    
    switch (op_) {
        case OperatorType::Add:
            result = leftNum + rightNum;
            break;
        case OperatorType::Subtract:
            result = leftNum - rightNum;
            break;
        case OperatorType::Multiply:
            result = leftNum * rightNum;
            // 乘法时，如果一个操作数无单位，使用另一个的单位
            if (left.unit.empty() && !right.unit.empty()) {
                resultUnit = right.unit;
            } else if (!left.unit.empty() && right.unit.empty()) {
                resultUnit = left.unit;
            } else if (!left.unit.empty() && !right.unit.empty()) {
                // 两个都有单位，单位相乘不符合CSS，返回calc
                std::stringstream ss;
                ss << "calc(" << left.toString() << " * " << right.toString() << ")";
                return CssUnit(ss.str(), "");
            }
            break;
        case OperatorType::Divide:
            if (rightNum != 0.0) {
                result = leftNum / rightNum;
            }
            // 除法时保留左侧单位
            resultUnit = left.unit;
            break;
        case OperatorType::Modulo:
            if (rightNum != 0.0) {
                result = std::fmod(leftNum, rightNum);
            }
            break;
        case OperatorType::Power:
            result = std::pow(leftNum, rightNum);
            break;
        // 比较运算符返回布尔值
        case OperatorType::Equal:
            return CssUnit(leftNum == rightNum ? "1" : "0", "");
        case OperatorType::NotEqual:
            return CssUnit(leftNum != rightNum ? "1" : "0", "");
        case OperatorType::Less:
            return CssUnit(leftNum < rightNum ? "1" : "0", "");
        case OperatorType::Greater:
            return CssUnit(leftNum > rightNum ? "1" : "0", "");
        case OperatorType::LessEqual:
            return CssUnit(leftNum <= rightNum ? "1" : "0", "");
        case OperatorType::GreaterEqual:
            return CssUnit(leftNum >= rightNum ? "1" : "0", "");
        case OperatorType::And:
            return CssUnit((leftNum != 0.0 && rightNum != 0.0) ? "1" : "0", "");
        case OperatorType::Or:
            return CssUnit((leftNum != 0.0 || rightNum != 0.0) ? "1" : "0", "");
    }
    
    // 格式化结果
    std::stringstream ss;
    if (result == static_cast<int>(result)) {
        ss << static_cast<int>(result);
    } else {
        ss << result;
    }
    
    return CssUnit(ss.str(), resultUnit);
}

// ConditionalExpression实现
String ConditionalExpression::evaluate() const {
    String condValue = condition_->evaluate();
    
    // 解析条件值
    double condNum = 0.0;
    try {
        String numStr;
        for (char c : condValue) {
            if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
                numStr += c;
            } else {
                break;
            }
        }
        condNum = std::stod(numStr);
    } catch (...) {
        condNum = 0.0;
    }
    
    if (condNum != 0.0) {
        return trueExpr_->evaluate();
    } else if (falseExpr_) {
        return falseExpr_->evaluate();
    }
    
    return "";
}

// SelectorExpression实现
String SelectorExpression::evaluate() const {
    // 从SaltBridge获取属性值
    using namespace Bridge;
    SaltBridge& bridge = SaltBridge::getInstance();
    
    // 查找元素的属性
    auto value = bridge.getElementProperty(selector_, property_);
    if (value.has_value()) {
        return value.value();
    }
    
    // 如果找不到，返回占位符注释
    return "/* " + selector_ + "." + property_ + " not found */";
}

// ExpressionParser实现
ExpressionParser::ExpressionParser(const Vector<Token>& tokens)
    : tokens_(tokens) {}

SharedPtr<Expression> ExpressionParser::parse() {
    return parseConditional();
}

String ExpressionParser::parseCssValue(const String& value) {
    // 简单处理：如果包含运算符，返回原样，否则返回值本身
    // 这个方法主要用于快速检测
    return value;
}

SharedPtr<Expression> ExpressionParser::parseConditional() {
    auto expr = parseLogicalOr();
    
    if (match(TokenType::Question)) {
        auto trueExpr = parseConditional();
        SharedPtr<Expression> falseExpr;
        
        if (match(TokenType::Colon)) {
            falseExpr = parseConditional();
        }
        
        return std::make_shared<ConditionalExpression>(expr, trueExpr, falseExpr);
    }
    
    return expr;
}

SharedPtr<Expression> ExpressionParser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (match(TokenType::Or)) {
        auto right = parseLogicalAnd();
        left = std::make_shared<BinaryExpression>(left, OperatorType::Or, right);
    }
    
    return left;
}

SharedPtr<Expression> ExpressionParser::parseLogicalAnd() {
    auto left = parseComparison();
    
    while (match(TokenType::And)) {
        auto right = parseComparison();
        left = std::make_shared<BinaryExpression>(left, OperatorType::And, right);
    }
    
    return left;
}

SharedPtr<Expression> ExpressionParser::parseComparison() {
    auto left = parseAdditive();
    
    while (true) {
        OperatorType op;
        
        if (match(TokenType::EqualEqual)) {
            op = OperatorType::Equal;
        } else if (match(TokenType::NotEqual)) {
            op = OperatorType::NotEqual;
        } else if (match(TokenType::LessEqual)) {
            op = OperatorType::LessEqual;
        } else if (match(TokenType::GreaterEqual)) {
            op = OperatorType::GreaterEqual;
        } else if (match(TokenType::Less)) {
            op = OperatorType::Less;
        } else if (match(TokenType::Greater)) {
            op = OperatorType::Greater;
        } else {
            break;
        }
        
        auto right = parseAdditive();
        left = std::make_shared<BinaryExpression>(left, op, right);
    }
    
    return left;
}

SharedPtr<Expression> ExpressionParser::parseAdditive() {
    auto left = parseMultiplicative();
    
    while (true) {
        OperatorType op;
        
        if (match(TokenType::Plus)) {
            op = OperatorType::Add;
        } else if (match(TokenType::Minus)) {
            op = OperatorType::Subtract;
        } else {
            break;
        }
        
        auto right = parseMultiplicative();
        left = std::make_shared<BinaryExpression>(left, op, right);
    }
    
    return left;
}

SharedPtr<Expression> ExpressionParser::parseMultiplicative() {
    auto left = parsePower();
    
    while (true) {
        OperatorType op;
        
        if (match(TokenType::Star)) {
            op = OperatorType::Multiply;
        } else if (match(TokenType::Slash)) {
            op = OperatorType::Divide;
        } else if (match(TokenType::Percent)) {
            op = OperatorType::Modulo;
        } else {
            break;
        }
        
        auto right = parsePower();
        left = std::make_shared<BinaryExpression>(left, op, right);
    }
    
    return left;
}

SharedPtr<Expression> ExpressionParser::parsePower() {
    auto left = parsePrimary();
    
    if (match(TokenType::Power)) {
        auto right = parsePower();  // 右结合
        left = std::make_shared<BinaryExpression>(left, OperatorType::Power, right);
    }
    
    return left;
}

SharedPtr<Expression> ExpressionParser::parsePrimary() {
    // 数字字面量
    if (check(TokenType::NumberLiteral)) {
        String value = advance().getValue();
        CssUnit unit = parseNumber(value);
        return std::make_shared<NumberExpression>(unit);
    }
    
    // 字符串字面量（在条件表达式的结果中常见）
    if (check(TokenType::StringLiteral)) {
        String value = advance().getValue();
        // 字符串字面量作为CSS值（如颜色名、关键字等）
        return std::make_shared<NumberExpression>(CssUnit(value, ""));
    }
    
    // .class或#id选择器开头的属性引用
    if (check(TokenType::Dot) || check(TokenType::Hash)) {
        String selector;
        if (match(TokenType::Dot)) {
            selector = ".";
            if (check(TokenType::Identifier)) {
                selector += advance().getValue();
            }
        } else if (match(TokenType::Hash)) {
            selector = "#";
            if (check(TokenType::Identifier)) {
                selector += advance().getValue();
            }
        }
        
        // 检查是否有.property
        if (match(TokenType::Dot)) {
            if (check(TokenType::Identifier)) {
                String property = advance().getValue();
                return std::make_shared<SelectorExpression>(selector, property);
            }
        }
        
        // 如果只是.class或#id而没有属性，返回为字面量（虽然不太可能）
        return std::make_shared<NumberExpression>(CssUnit(selector, ""));
    }
    
    // 标识符或HTML关键字（属性或选择器引用）
    // 注意：HTML关键字也可以作为选择器（如div.width, base.height等）
    if (check(TokenType::Identifier) || check(TokenType::UnquotedLiteral) || check(TokenType::HtmlKeyword)) {
        String identifier = advance().getValue();
        
        // 检查是否是选择器引用 (selector.property)
        if (match(TokenType::Dot)) {
            if (check(TokenType::Identifier) || check(TokenType::HtmlKeyword)) {
                String property = advance().getValue();
                return std::make_shared<SelectorExpression>(identifier, property);
            }
        }
        
        // 检查是否是CSS关键字（如 block, none, red, blue等）
        // 这些应该直接返回作为值，而不是属性引用
        static const std::unordered_set<String> cssKeywords = {
            "block", "none", "inline", "flex", "grid", "auto", "inherit", "initial",
            "red", "blue", "green", "white", "black", "transparent", "currentColor",
            "center", "left", "right", "top", "bottom", "middle",
            "solid", "dotted", "dashed", "double", "hidden"
        };
        
        if (cssKeywords.find(identifier) != cssKeywords.end()) {
            // CSS关键字作为字面量值
            return std::make_shared<NumberExpression>(CssUnit(identifier, ""));
        }
        
        // 属性引用
        return std::make_shared<PropertyExpression>(identifier);
    }
    
    // 括号表达式
    if (match(TokenType::LeftParen)) {
        auto expr = parseConditional();
        match(TokenType::RightParen);
        return expr;
    }
    
    // 默认返回0
    return std::make_shared<NumberExpression>(CssUnit("0", ""));
}

CssUnit ExpressionParser::parseNumber(const String& str) {
    String numStr;
    String unit;
    bool foundNum = false;
    
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
            numStr += c;
            foundNum = true;
        } else if (foundNum) {
            unit = str.substr(i);
            break;
        }
    }
    
    return CssUnit(numStr, unit);
}

Token ExpressionParser::getCurrentToken() const {
    if (current_ < tokens_.size()) {
        return tokens_[current_];
    }
    return tokens_.back();
}

Token ExpressionParser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return tokens_[current_ - 1];
}

Token ExpressionParser::peek(int offset) const {
    size_t index = current_ + offset;
    if (index < tokens_.size()) {
        return tokens_[index];
    }
    return tokens_.back();
}

bool ExpressionParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool ExpressionParser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return getCurrentToken().is(type);
}

bool ExpressionParser::isAtEnd() const {
    return current_ >= tokens_.size() || getCurrentToken().is(TokenType::EndOfFile);
}

} // namespace CHTL
