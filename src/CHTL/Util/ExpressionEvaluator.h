#pragma once

#include "../CHTLNode/StyleValue.h"
#include "../CHTLParser/Parser.h" // For parser context
#include "../CHTLNode/ElementNode.h" // For context node
#include "Selector.h" // For the Selector struct
#include <vector>
#include <memory>

class ExpressionEvaluator {
public:
    // The parser context is needed to look up things like other nodes, templates, etc.
    ExpressionEvaluator(Parser& parser, ElementNode* contextNode);

    // Main entry point for evaluating an expression. It will manage the internal token state.
    StyleValue evaluate();

    // A specific entry point for parsing just a single boolean condition.
    StyleValue evaluateBoolean();

private:
    Parser& parser; // Reference to the main parser for token stream and context
    ElementNode* contextNode; // The element in which the expression is being evaluated

    // --- All the expression parsing methods from StyleBlockState will be moved here ---
    StyleValue parseConditionalExpr();
    StyleValue parseBooleanOrExpr();
    StyleValue parseBooleanAndExpr();
    StyleValue parseBooleanRelationalExpr();
    StyleValue parseAdditiveExpr();
    StyleValue parseMultiplicativeExpr();
    StyleValue parsePrimaryExpr();
    StyleValue parseReferencedProperty();
    Selector parseSelector();
};