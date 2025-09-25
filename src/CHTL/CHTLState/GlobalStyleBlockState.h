#pragma once

#include "ParserState.h"

// State for parsing the contents of a global 'style { ... }' block.
// This state is similar to StyleBlockState but adapted for the global scope,
// where there is no parent element context.
class GlobalStyleBlockState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};