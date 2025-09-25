#pragma once

#include "ParserState.h"
#include "../CHTLNode/StyleValue.h"
#include "../Util/Selector.h"

// State for parsing the contents of a global 'style { ... }' block.
// This state is similar to StyleBlockState but adapted for the global scope,
// where there is no parent element context.
class GlobalStyleBlockState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;

private:
    // Expression parsing methods, adapted from StyleBlockState
    StyleValue parseStyleExpression(Parser& parser);
    StyleValue parseConditionalExpr(Parser& parser);
    StyleValue parseBooleanOrExpr(Parser& parser);
    StyleValue parseBooleanAndExpr(Parser& parser);
    StyleValue parseBooleanRelationalExpr(Parser& parser);
    StyleValue parseAdditiveExpr(Parser& parser);
    StyleValue parseMultiplicativeExpr(Parser& parser);
    StyleValue parsePrimaryExpr(Parser& parser);
    StyleValue parseReferencedProperty(Parser& parser);
    Selector parseSelector(Parser& parser);
};