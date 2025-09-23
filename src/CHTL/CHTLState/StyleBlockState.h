#pragma once

#include "ParserState.h"
#include "../CHTLNode/StyleValue.h"
#include <string>
#include <utility> // For std::pair

// Forward declare to avoid circular dependencies
class ElementNode;

// State for parsing the contents of a 'style { ... }' block.
// This state handles parsing both inline styles and selector-based rules.
class StyleBlockState : public ParserState {
public:
    // The handle method will parse the entire style block.
    // It doesn't return a node, but modifies the parser's contextNode
    // (the element containing the style block) and potentially the parser's
    // collection of global styles.
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    void parseStyleTemplateUsage(Parser& parser);
    void parseClassOrIdSelector(Parser& parser);
    void parseAmpersandSelector(Parser& parser);
    std::string parseCssRuleBlock(Parser& parser);
    void parseInlineProperty(Parser& parser);

    // --- Expression Parsing ---
    // Top-level entry point for parsing a style value.
    StyleValue parseStyleExpression(Parser& parser);

private:
    // --- Conditional Expression Parsing ---
    // Parses a full conditional chain: cond ? val, cond ? val ...
    StyleValue parseConditionalExpr(Parser& parser);

    // --- Boolean Condition Parsing (for inside conditionals) ---
    StyleValue parseBooleanOrExpr(Parser& parser);
    StyleValue parseBooleanAndExpr(Parser& parser);
    StyleValue parseBooleanRelationalExpr(Parser& parser);

    // --- Style Value Parsing (used by both conditional and simple expressions) ---
    StyleValue parseAdditiveExpr(Parser& parser);
    StyleValue parseMultiplicativeExpr(Parser& parser);
    StyleValue parsePrimaryExpr(Parser& parser);
    StyleValue parseReferencedProperty(Parser& parser);
};
