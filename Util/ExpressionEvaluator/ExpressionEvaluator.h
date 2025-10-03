#ifndef CHTL_EXPRESSION_EVALUATOR_H
#define CHTL_EXPRESSION_EVALUATOR_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <CHTL/CHTLLexer/Token.h>

class ElementNode; // Forward declaration

class ExpressionEvaluator {
public:
    explicit ExpressionEvaluator(const ElementNode& context);
    bool evaluate(const std::vector<Token>& tokens);

private:
    const ElementNode& context;

    std::vector<Token> shuntingYard(const std::vector<Token>& tokens);
    double evaluateRPN(const std::vector<Token>& rpnTokens);
    double getValue(const std::string& identifier);
    int getPrecedence(TokenType type);
    bool isOperator(TokenType type);
};

#endif // CHTL_EXPRESSION_EVALUATOR_H