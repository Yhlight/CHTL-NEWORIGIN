#pragma once

#include "ParserState.h"
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
    void parseClassOrIdSelector(Parser& parser);
    void parseAmpersandSelector(Parser& parser);
    std::string parseCssRuleBlock(Parser& parser);
    void parseInlineProperty(Parser& parser);

    // Expression parsing logic for style property values.
    std::string parseStyleExpression(Parser& parser);
    std::pair<double, std::string> parseAdditiveExpr(Parser& parser);
    std::pair<double, std::string> parseMultiplicativeExpr(Parser& parser);
    std::pair<double, std::string> parsePrimaryExpr(Parser& parser);
};
