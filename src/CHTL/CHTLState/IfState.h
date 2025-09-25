#pragma once

#include "ParserState.h"

class IfState : public ParserState {
public:
    std::unique_ptr<BaseNode> handle(Parser& parser) override;
};