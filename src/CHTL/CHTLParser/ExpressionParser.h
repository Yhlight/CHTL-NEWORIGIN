#ifndef CHTL_EXPRESSION_PARSER_H
#define CHTL_EXPRESSION_PARSER_H

#include "../../Common.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

// 表达式类型
enum class ExpressionType {
    Number,        // 数字字面量
    Property,      // 属性引用
    Binary,        // 二元运算
    Conditional,   // 条件表达式
    Selector,      // 选择器引用
};

// 运算符类型
enum class OperatorType {
    Add,           // +
    Subtract,      // -
    Multiply,      // *
    Divide,        // /
    Modulo,        // %
    Power,         // **
    Equal,         // ==
    NotEqual,      // !=
    Less,          // <
    Greater,       // >
    LessEqual,     // <=
    GreaterEqual,  // >=
    And,           // &&
    Or,            // ||
};

// CSS单位
struct CssUnit {
    String value;
    String unit;  // px, %, em, rem, vh, vw等
    
    CssUnit() = default;
    CssUnit(const String& v, const String& u = "") : value(v), unit(u) {}
    
    String toString() const {
        return value + unit;
    }
    
    double toDouble() const;
    bool hasUnit() const { return !unit.empty(); }
};

// 表达式节点基类
class Expression {
public:
    virtual ~Expression() = default;
    virtual ExpressionType getType() const = 0;
    virtual String evaluate() const = 0;
    virtual SharedPtr<Expression> clone() const = 0;
};

// 数字表达式
class NumberExpression : public Expression {
public:
    NumberExpression(const CssUnit& value) : value_(value) {}
    
    ExpressionType getType() const override { return ExpressionType::Number; }
    String evaluate() const override { return value_.toString(); }
    SharedPtr<Expression> clone() const override {
        return std::make_shared<NumberExpression>(value_);
    }
    
    const CssUnit& getValue() const { return value_; }
    
private:
    CssUnit value_;
};

// 属性引用表达式
class PropertyExpression : public Expression {
public:
    PropertyExpression(const String& property) : property_(property) {}
    
    ExpressionType getType() const override { return ExpressionType::Property; }
    String evaluate() const override;
    SharedPtr<Expression> clone() const override {
        return std::make_shared<PropertyExpression>(property_);
    }
    
    const String& getProperty() const { return property_; }
    
private:
    String property_;
};

// 二元运算表达式
class BinaryExpression : public Expression {
public:
    BinaryExpression(SharedPtr<Expression> left, OperatorType op, SharedPtr<Expression> right)
        : left_(left), op_(op), right_(right) {}
    
    ExpressionType getType() const override { return ExpressionType::Binary; }
    String evaluate() const override;
    SharedPtr<Expression> clone() const override {
        return std::make_shared<BinaryExpression>(left_->clone(), op_, right_->clone());
    }
    
    const SharedPtr<Expression>& getLeft() const { return left_; }
    const SharedPtr<Expression>& getRight() const { return right_; }
    OperatorType getOperator() const { return op_; }
    
private:
    SharedPtr<Expression> left_;
    OperatorType op_;
    SharedPtr<Expression> right_;
    
    CssUnit evaluateNumeric() const;
};

// 条件表达式
class ConditionalExpression : public Expression {
public:
    ConditionalExpression(SharedPtr<Expression> condition,
                         SharedPtr<Expression> trueExpr,
                         SharedPtr<Expression> falseExpr = nullptr)
        : condition_(condition), trueExpr_(trueExpr), falseExpr_(falseExpr) {}
    
    ExpressionType getType() const override { return ExpressionType::Conditional; }
    String evaluate() const override;
    SharedPtr<Expression> clone() const override {
        return std::make_shared<ConditionalExpression>(
            condition_->clone(),
            trueExpr_->clone(),
            falseExpr_ ? falseExpr_->clone() : nullptr
        );
    }
    
private:
    SharedPtr<Expression> condition_;
    SharedPtr<Expression> trueExpr_;
    SharedPtr<Expression> falseExpr_;
};

// 选择器引用表达式 (box.width)
class SelectorExpression : public Expression {
public:
    SelectorExpression(const String& selector, const String& property)
        : selector_(selector), property_(property) {}
    
    ExpressionType getType() const override { return ExpressionType::Selector; }
    String evaluate() const override;
    SharedPtr<Expression> clone() const override {
        return std::make_shared<SelectorExpression>(selector_, property_);
    }
    
    const String& getSelector() const { return selector_; }
    const String& getProperty() const { return property_; }
    
private:
    String selector_;
    String property_;
};

// 表达式解析器
class ExpressionParser {
public:
    ExpressionParser(const Vector<Token>& tokens);
    
    // 解析表达式
    SharedPtr<Expression> parse();
    
    // 解析CSS值（可能包含表达式）
    static String parseCssValue(const String& value);
    
private:
    SharedPtr<Expression> parseConditional();
    SharedPtr<Expression> parseLogicalOr();
    SharedPtr<Expression> parseLogicalAnd();
    SharedPtr<Expression> parseComparison();
    SharedPtr<Expression> parseAdditive();
    SharedPtr<Expression> parseMultiplicative();
    SharedPtr<Expression> parsePower();
    SharedPtr<Expression> parsePrimary();
    
    CssUnit parseNumber(const String& str);
    
    Token getCurrentToken() const;
    Token advance();
    Token peek(int offset = 1) const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    bool isAtEnd() const;
    
    Vector<Token> tokens_;
    size_t current_ = 0;
};

} // namespace CHTL

#endif // CHTL_EXPRESSION_PARSER_H
