#pragma once

#include "ParserState.h"
#include <string>
#include <utility> // For std::pair

// Forward declare to avoid circular dependencies
class ElementNode;

// State for parsing the contents of a 'style { ... }' block.
// This state handles parsing both inline styles and selector-based rules.
struct StyleValue; // Forward declaration
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

    // Expression parsing logic for style property values.
    StyleValue parseStyleExpression(Parser& parser);
    StyleValue parseTernaryExpr(Parser& parser);
    StyleValue parseLogicalOrExpr(Parser& parser);
    StyleValue parseLogicalAndExpr(Parser& parser);
    StyleValue parseEqualityExpr(Parser& parser);
    StyleValue parseComparisonExpr(Parser& parser);
    StyleValue parseAdditiveExpr(Parser& parser);
    StyleValue parseMultiplicativeExpr(Parser& parser);
    StyleValue parsePrimaryExpr(Parser& parser);
};
