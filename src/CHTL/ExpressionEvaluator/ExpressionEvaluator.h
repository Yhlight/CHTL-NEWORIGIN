#pragma once

#include "../ExpressionNode/ExpressionBaseNode.h"
#include <string>
#include <memory>

// Forward declarations
class Parser;
class LiteralNode;
class PropertyRefNode;
class BinaryOpNode;
class ConditionalNode;

// The ExpressionEvaluator traverses an expression AST and computes a final value.
// For now, it will return a string result. A more advanced version would
// return a structured value with type and unit information.
class ExpressionEvaluator {
public:
    // The context of the main parser is needed to resolve property references.
    explicit ExpressionEvaluator(Parser& context);

    // The main entry point for evaluation.
    std::string evaluate(ExpressionBaseNode* node);

private:
    Parser& parserContext;

    // Visitor methods for each node type in the expression AST.
    std::string visit(ExpressionBaseNode* node);
    std::string visitLiteralNode(LiteralNode* node);
    std::string visitPropertyRefNode(PropertyRefNode* node);
    std::string visitBinaryOpNode(BinaryOpNode* node);
    std::string visitConditionalNode(ConditionalNode* node);
};