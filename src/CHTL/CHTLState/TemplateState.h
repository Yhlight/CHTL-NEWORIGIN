#pragma once

#include "ParserState.h"

// State for parsing a [Template] or [Custom] block definition.
class TemplateState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};