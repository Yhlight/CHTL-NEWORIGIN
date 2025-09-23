#include "StyleEvaluator.h"
#include "../CHTLExpr/BinaryOpNode.h"
#include "../CHTLExpr/TernaryOpNode.h"
#include "../CHTLExpr/PropertyAccessNode.h"
#include "../CHTLExpr/IdentifierNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <vector>
#include <queue>
#include <set>

// A variant-like struct to hold the result of an evaluation
struct EvaluatedValue {
    bool isNumeric;
    ParsedValue numericVal;
    std::string stringVal;
};

// Helper to convert the numeric part of an EvaluatedValue to a string
std::string valueToString(const EvaluatedValue& ev) {
    if (!ev.isNumeric) return ev.stringVal;
    const ParsedValue& pv = ev.numericVal;
    // Check for integer values to avoid trailing zeros
    if (pv.value == floor(pv.value)) {
        return std::to_string(static_cast<int>(pv.value)) + pv.unit;
    }
    return std::to_string(pv.value) + pv.unit;
}

// New helper to find dependencies in an expression
void findDependencies(const std::shared_ptr<BaseExprNode>& expr, const std::map<std::string, std::shared_ptr<BaseExprNode>>& styles, std::set<std::string>& deps) {
    if (!expr) return;
    switch (expr->getType()) {
        case ExprNodeType::Identifier: {
            auto ident = std::dynamic_pointer_cast<IdentifierNode>(expr);
            if (styles.count(ident->name)) { // Check if the identifier is a known property
                deps.insert(ident->name);
            }
            break;
        }
        case ExprNodeType::PropertyAccess: {
            // Cross-element dependencies are not handled in this pass.
            // They should be resolved by the CHTLResolver before evaluation.
            break;
        }
        case ExprNodeType::BinaryOp: {
            auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr);
            findDependencies(binOp->left, styles, deps);
            findDependencies(binOp->right, styles, deps);
            break;
        }
        case ExprNodeType::TernaryOp: {
            auto terOp = std::dynamic_pointer_cast<TernaryOpNode>(expr);
            findDependencies(terOp->condition, styles, deps);
            findDependencies(terOp->trueExpr, styles, deps);
            findDependencies(terOp->falseExpr, styles, deps);
            break;
        }
        default:
            // Literal nodes have no dependencies
            break;
    }
}

void StyleEvaluator::evaluate(const std::shared_ptr<ElementNode>& root) {
    if (!root) return;
    evaluateNode(root);
}

void StyleEvaluator::evaluateNode(const std::shared_ptr<ElementNode>& node) {
    const auto& stylesToProcess = node->inlineStyles;

    // 1. Graph Building Phase
    std::map<std::string, std::set<std::string>> adjList; // u -> v means u is a dependency for v
    std::map<std::string, int> inDegree;

    for (const auto& pair : stylesToProcess) {
        inDegree[pair.first] = 0;
    }

    for (const auto& pair : stylesToProcess) {
        std::set<std::string> dependencies;
        findDependencies(pair.second, stylesToProcess, dependencies);
        for (const auto& dep : dependencies) {
            if (stylesToProcess.count(dep)) {
                adjList[dep].insert(pair.first);
                inDegree[pair.first]++;
            }
        }
    }

    // 2. Topological Sort (Kahn's Algorithm)
    std::queue<std::string> q;
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            q.push(pair.first);
        }
    }

    std::vector<std::string> sortedOrder;
    while (!q.empty()) {
        std::string u = q.front();
        q.pop();
        sortedOrder.push_back(u);

        if (adjList.count(u)) {
            for (const auto& v : adjList[u]) {
                inDegree[v]--;
                if (inDegree[v] == 0) {
                    q.push(v);
                }
            }
        }
    }

    // 3. Cycle Detection & Evaluation
    std::map<std::string, EvaluatedValue> localContext;
    if (sortedOrder.size() != stylesToProcess.size()) {
        for (auto const& [prop, expr] : stylesToProcess) {
            if (inDegree[prop] > 0) {
                node->finalStyles[prop] = "ERROR: Circular dependency detected.";
            }
        }
    }

    // 4. Evaluation Phase
    for (const auto& propName : sortedOrder) {
        try {
            auto expr = stylesToProcess.at(propName);
            localContext[propName] = evaluateExpression(expr, localContext);
            node->finalStyles[propName] = valueToString(localContext[propName]);
        } catch (const std::exception& e) {
            node->finalStyles[propName] = "ERROR: " + std::string(e.what());
        }
    }

    // Recursively evaluate for all children
    for (const auto& child : node->children) {
        if (child->getType() == NodeType::Element) {
            evaluateNode(std::dynamic_pointer_cast<ElementNode>(child));
        }
    }
}

EvaluatedValue StyleEvaluator::evaluateExpression(const std::shared_ptr<BaseExprNode>& expr, const std::map<std::string, EvaluatedValue>& context) {
    if (!expr) throw std::runtime_error("Cannot evaluate null expression.");

    switch (expr->getType()) {
        case ExprNodeType::Literal: {
            auto literal = std::dynamic_pointer_cast<LiteralNode>(expr);
            if (literal->isNumeric) return {true, literal->numericValue, ""};
            return {false, {}, literal->rawValue};
        }
        case ExprNodeType::Identifier: {
            auto ident = std::dynamic_pointer_cast<IdentifierNode>(expr);
            if (context.count(ident->name)) {
                return context.at(ident->name);
            } else {
                return {false, {}, ident->name};
            }
        }
        case ExprNodeType::PropertyAccess: {
            auto access = std::dynamic_pointer_cast<PropertyAccessNode>(expr);
            std::string errorMsg = "ERROR: Cross-element access ('" + access->selector + "." + access->propertyName + "') is not supported yet.";
            return {false, {}, errorMsg};
        }
        case ExprNodeType::TernaryOp: {
            auto ternOp = std::dynamic_pointer_cast<TernaryOpNode>(expr);
            EvaluatedValue cond = evaluateExpression(ternOp->condition, context);
            if (!cond.isNumeric) throw std::runtime_error("Condition for ternary must be numeric.");
            if (cond.numericVal.value != 0) {
                return evaluateExpression(ternOp->trueExpr, context);
            } else {
                if (ternOp->falseExpr) {
                    return evaluateExpression(ternOp->falseExpr, context);
                } else {
                    // Optional false branch evaluates to empty string
                    return {false, {}, ""};
                }
            }
        }
        case ExprNodeType::BinaryOp: {
            auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr);
            EvaluatedValue left = evaluateExpression(binOp->left, context);
            EvaluatedValue right = evaluateExpression(binOp->right, context);

            if (!left.isNumeric || !right.isNumeric) {
                 if (binOp->op == TokenType::EQUAL_EQUAL) return {true, { (double)(left.stringVal == right.stringVal), ""}, ""};
                 if (binOp->op == TokenType::NOT_EQUAL) return {true, { (double)(left.stringVal != right.stringVal), ""}, ""};
                 if (binOp->op == TokenType::PLUS) {
                    return {false, {}, valueToString(left) + valueToString(right)};
                 }
                 throw std::runtime_error("Invalid operands for binary operation.");
            }

            const ParsedValue& leftPv = left.numericVal;
            const ParsedValue& rightPv = right.numericVal;

            if (!leftPv.unit.empty() && !rightPv.unit.empty() && leftPv.unit != rightPv.unit) {
                throw std::runtime_error("Incompatible units: " + leftPv.unit + " and " + rightPv.unit);
            }
            std::string resultUnit = !leftPv.unit.empty() ? leftPv.unit : rightPv.unit;
            double resultValue = 0;

            switch (binOp->op) {
                case TokenType::PLUS: resultValue = leftPv.value + rightPv.value; break;
                case TokenType::MINUS: resultValue = leftPv.value - rightPv.value; break;
                case TokenType::STAR: resultValue = leftPv.value * rightPv.value; break;
                case TokenType::SLASH:
                    if (rightPv.value == 0) throw std::runtime_error("Division by zero.");
                    resultValue = leftPv.value / rightPv.value; break;
                case TokenType::PERCENT: resultValue = fmod(leftPv.value, rightPv.value); break;
                case TokenType::DOUBLE_STAR: resultValue = pow(leftPv.value, rightPv.value); break;
                case TokenType::GREATER: resultValue = leftPv.value > rightPv.value; break;
                case TokenType::LESS: resultValue = leftPv.value < rightPv.value; break;
                case TokenType::EQUAL_EQUAL: resultValue = leftPv.value == rightPv.value; break;
                case TokenType::NOT_EQUAL: resultValue = leftPv.value != rightPv.value; break;
                case TokenType::LOGICAL_AND: resultValue = leftPv.value && rightPv.value; break;
                case TokenType::LOGICAL_OR: resultValue = leftPv.value || rightPv.value; break;
                default: throw std::runtime_error("Unsupported binary operator.");
            }
            return {true, {resultValue, resultUnit}, ""};
        }
        default:
            throw std::runtime_error("Unsupported expression node type for evaluation.");
    }
}

void StyleEvaluator::evaluateGlobalRules(CHTLContext& context) {
    for (const auto& ruleNode : context.globalCssRuleNodes) {
        std::string ruleString = ruleNode->selector + " { ";
        for (const auto& propPair : ruleNode->properties) {
            try {
                std::map<std::string, EvaluatedValue> emptyContext;
                EvaluatedValue result = evaluateExpression(propPair.second, emptyContext);
                ruleString += propPair.first + ": " + valueToString(result) + "; ";
            } catch (const std::exception& e) {
                 ruleString += "/* ERROR evaluating " + propPair.first + ": " + e.what() + " */ ";
            }
        }
        ruleString += "}";
        context.finalGlobalCssRules.push_back(ruleString);
    }
}
